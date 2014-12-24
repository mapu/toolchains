# Defines basic Tcl procs that must exist for OpenOCD scripts to work.
#
# Embedded into OpenOCD executable, thus it cannot be modified casually.
#


# We need to explicitly redirect this to the OpenOCD command
# as Tcl defines the exit proc
proc exit {} {
	ocd_throw exit
}

# All commands are registered with an 'ocd_' prefix, while the "real"
# command is a wrapper that calls this function.  Its primary purpose is
# to discard 'handler' command output,
proc ocd_bouncer {name args} {
	set cmd [format "ocd_%s" $name]
	set type [eval ocd_command type $cmd $args]
	if {$type == "native"} {
		return [eval $cmd $args]
	} else {if {$type == "simple"} {
		if {[catch {eval $cmd $args}] == 0} {
			return ""
		} else {
			# 'classic' commands output error message as part of progress output
			set errmsg ""
		}
	} else {if {$type == "group"} {
		catch {eval ocd_usage $name $args}
		set errmsg [format "%s: command requires more arguments" \
			[concat $name " " $args]]
	} else {
		set errmsg [format "Unknown command type: %s" $type]
	}}}
	return -code error $errmsg
}

# Try flipping / and \ to find file if the filename does not
# match the precise spelling
proc find {filename} {
	if {[catch {ocd_find $filename} t]==0} {
		return $t
	}
	if {[catch {ocd_find [string map {\ /} $filename} t]==0} {
		return $t
	}
	if {[catch {ocd_find [string map {/ \\} $filename} t]==0} {
		return $t
	}
	# make sure error message matches original input string
	return -code error "Can't find $filename"
}
add_usage_text find "<file>"
add_help_text find "print full path to file according to OpenOCD search rules"

# Run script
proc script {filename} {
	source [find $filename]
}
add_help_text script "filename of OpenOCD script (tcl) to run"
add_usage_text script "<file>"

#########

# Defines basic Tcl procs for OpenOCD JTAG module

# Executed during "init". Can be overridden
# by board/target/... scripts
proc jtag_init {} {
	if {[catch {jtag arp_init} err]!=0} {
		# try resetting additionally
		init_reset startup
	}
}

# This reset logic may be overridden by board/target/... scripts as needed
# to provide a reset that, if possible, is close to a power-up reset.
#
# Exit requirements include:  (a) JTAG must be working, (b) the scan
# chain was validated with "jtag arp_init" (or equivalent), (c) nothing
# stays in reset.  No TAP-specific scans were performed.  It's OK if
# some targets haven't been reset yet; they may need TAP-specific scans.
#
# The "mode" values include:  halt, init, run (from "reset" command);
# startup (at OpenOCD server startup, when JTAG may not yet work); and
# potentially more (for reset types like cold, warm, etc)
proc init_reset { mode } {
	jtag arp_init-reset
}

#########

# TODO: power_restore and power_dropout are currently neither
# documented nor supported except on ZY1000.

proc power_restore {} {
	echo "Sensed power restore, running reset init and halting GDB."
	reset init
	
	# Halt GDB so user can deal with a detected power restore.
	#
	# After GDB is halted, then output is no longer forwarded
	# to the GDB console.
	set targets [target names]	
	foreach t $targets {
		# New event script.
		$t invoke-event arp_halt_gdb
	}	
}

add_help_text power_restore "Overridable procedure run when power restore is detected. Runs 'reset init' by default."

proc power_dropout {} {
	echo "Sensed power dropout."
}

#########

# TODO: srst_deasserted and srst_asserted are currently neither
# documented nor supported except on ZY1000.

proc srst_deasserted {} {
	echo "Sensed nSRST deasserted, running reset init and halting GDB."
	reset init

	# Halt GDB so user can deal with a detected reset.
	#
	# After GDB is halted, then output is no longer forwarded
	# to the GDB console.
	set targets [target names]	
	foreach t $targets {
		# New event script.
		$t invoke-event arp_halt_gdb
	}		
}

add_help_text srst_deasserted "Overridable procedure run when srst deassert is detected. Runs 'reset init' by default."

proc srst_asserted {} {
	echo "Sensed nSRST asserted."
}

# measure actual JTAG clock
proc measure_clk {} {
	set start_time [ms];
	runtest 10000000; 
	echo "Running at more than [expr 10000.0 / ([ms]-$start_time)] kHz";
}

add_help_text measure_clk "Runs a test to measure the JTAG clk. Useful with RCLK / RTCK."

# BEGIN MIGRATION AIDS ...  these adapter operations originally had
# JTAG-specific names despite the fact that the operations were not
# specific to JTAG, or otherewise had troublesome/misleading names.
#
# FIXME phase these aids out after about April 2011
#
proc jtag_khz args { eval adapter_khz $args }
proc jtag_nsrst_delay args { eval adapter_nsrst_delay $args }
proc jtag_nsrst_assert_width args { eval adapter_nsrst_assert_width $args }

# END MIGRATION AIDS
# Defines basic Tcl procs for OpenOCD target module

proc new_target_name { } {
	return [target number [expr [target count] - 1 ]]
}

# 1 means ocd is already in reset procedure
global in_process_reset
set in_process_reset 0

# Catch reset recursion
proc ocd_process_reset { MODE } {
	global in_process_reset
	if {$in_process_reset} {
		set in_process_reset 0
		return -code error "'reset' can not be invoked recursively"
	}

	set in_process_reset 1
	set success [expr [catch {ocd_process_reset_inner $MODE} result]==0]
	set in_process_reset 0

	if {$success} {
		return $result
	} else {
		return -code error $result
	}
}

proc ocd_process_reset_inner { MODE } {
	set targets [target names]

	# If this target must be halted...
	set halt -1
	if { 0 == [string compare $MODE halt] } {
		set halt 1
	}
	if { 0 == [string compare $MODE init] } {
		set halt 1;
	}
	if { 0 == [string compare $MODE run ] } {
		set halt 0;
	}
	if { $halt < 0 } {
		return -code error "Invalid mode: $MODE, must be one of: halt, init, or run";
	}

	# Target event handlers *might* change which TAPs are enabled
	# or disabled, so we fire all of them.  But don't issue any
	# target "arp_*" commands, which may issue JTAG transactions,
	# unless we know the underlying TAP is active.
	#
	# NOTE:  ARP == "Advanced Reset Process" ... "advanced" is
	# relative to a previous restrictive scheme

	foreach t $targets {
		# New event script.
		$t invoke-event reset-start
	}

	# Use TRST or TMS/TCK operations to reset all the tap controllers.
	# TAP reset events get reported; they might enable some taps.
	# This is a jtag-specific command, since it refers to tcl command "jtag arp_init-reset"
	#init_reset $MODE

	# Examine all targets on enabled taps.
	foreach t $targets {
	#	if {[jtag tapisenabled [$t cget -chain-position]]} {
			$t arp_examine
	#	}
	}

	# Assert SRST, and report the pre/post events.
	# Note:  no target sees SRST before "pre" or after "post".
	foreach t $targets {
		$t invoke-event reset-assert-pre
	}
	foreach t $targets {
		# C code needs to know if we expect to 'halt'
	#	if {[jtag tapisenabled [$t cget -chain-position]]} {
			$t arp_reset assert $halt
	#	}
	}
	foreach t $targets {
		$t invoke-event reset-assert-post
	}

	# Now de-assert SRST, and report the pre/post events.
	# Note:  no target sees !SRST before "pre" or after "post".
	foreach t $targets {
		$t invoke-event reset-deassert-pre
	}
	foreach t $targets {
		# Again, de-assert code needs to know if we 'halt'
	#	if {[jtag tapisenabled [$t cget -chain-position]]} {
			$t arp_reset deassert $halt
	#	}
	}
	foreach t $targets {
		$t invoke-event reset-deassert-post
	}

	# Pass 1 - Now wait for any halt (requested as part of reset
	# assert/deassert) to happen.  Ideally it takes effect without
	# first executing any instructions.
	if { $halt } {
		foreach t $targets {
			#if {[jtag tapisenabled [$t cget -chain-position]] == 0} {
			#	continue
			#}

			# Wait upto 1 second for target to halt.  Why 1sec? Cause
			# the JTAG tap reset signal might be hooked to a slow
			# resistor/capacitor circuit - and it might take a while
			# to charge

			# Catch, but ignore any errors.
			catch { $t arp_waitstate halted 1000 }

			# Did we succeed?
			set s [$t curstate]

			if { 0 != [string compare $s "halted" ] } {
				return -code error [format "TARGET: %s - Not halted" $t]
			}
		}
	}

	#Pass 2 - if needed "init"
	if { 0 == [string compare init $MODE] } {
		foreach t $targets {
			#if {[jtag tapisenabled [$t cget -chain-position]] == 0} {
			#	continue
			#}

			set err [catch "$t arp_waitstate halted 5000"]
			# Did it halt?
			if { $err == 0 } {
				$t invoke-event reset-init
			}
		}
	}

	foreach t $targets {
		$t invoke-event reset-end
	}
}

#########

# Temporary migration aid.  May be removed starting in January 2011.
proc armv4_5 params {
	echo "DEPRECATED! use 'arm $params' not 'armv4_5 $params'"
	arm $params
}

# Target/chain configuration scripts can either execute commands directly 
# or define a procedure which is executed once all configuration 
# scripts have completed.
#
# By default(classic) the config scripts will set up the target configuration
proc init_targets {} {
}
# Defines basic Tcl procs for OpenOCD flash module

# ease migration to updated flash driver
proc stm32x args {
	echo "DEPRECATED! use 'stm32f1x $args' not 'stm32x $args'"
	eval stm32f1x $args
}

proc stm32f2xxx args {
	echo "DEPRECATED! use 'stm32f2x $args' not 'stm32f2xxx $args'"
	eval stm32f2x $args
}

# Defines basic Tcl procs for OpenOCD server modules

# Handle GDB 'R' packet. Can be overridden by configuration script,
# but it's not something one would expect target scripts to do
# normally
proc ocd_gdb_restart {target_id} {
	# Fix!!! we're resetting all targets here! Really we should reset only
	# one target
	reset halt
}
