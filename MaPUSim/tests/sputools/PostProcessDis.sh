#!/bin/sh
#PostProcessDis.sh

if [ -f app.s.o ]
then
  readelf -s  app.s.o > symbol.temp0.s.sym
  echo "Read the object SYMBOL success"
  # del first 2 lines
  awk '$0 ~/^Symbol/ {no=NR} {if(NR>no) print > "symbol.temp1.s.sym"}' symbol.temp0.s.sym

  # del the unused coloum
  awk  '{$1=""; print > "symbol.temp0.s.sym"}'  symbol.temp1.s.sym
  awk  '{$2=""; print > "symbol.temp1.s.sym"}'  symbol.temp0.s.sym
  awk  '{$2=""; print > "symbol.temp0.s.sym"}'  symbol.temp1.s.sym
  awk  '{$2=""; print > "symbol.temp1.s.sym"}'  symbol.temp0.s.sym
  awk  '{$2=""; print > "symbol.temp0.s.sym"}'  symbol.temp1.s.sym
  awk  '{$2=""; print > "symbol.temp1.s.sym"}'  symbol.temp0.s.sym

  # del the null 4 lines and unused temp files
  awk '$0 ~/^Value/ {no=NR} {if(NR>no+4) print > "symbol.s.sym"}' symbol.temp1.s.sym

  # del the unused lines in dis
  awk '$0 ~/^app/ {no=NR} {if(NR>no+3) print > "app.temp.s.dis"}' app.s.dis
  awk  '{ {gsub(":", "")} ; print >"app.s.dis"}' app.temp.s.dis
  rm -rf symbol.temp0.s.sym symbol.temp1.s.sym app.temp.s.dis

  # process the disassemble file and replace the immediate with label in app.s.dis
  # add the label
  awk  'NR==FNR {Table[$1]=$2} NR>FNR {for(i in Table) if(strtonum("0x" i)==strtonum("0x"$1)) {print Table[i] "\n" $0 >> "YLZTemp.dis"; flag = 0; break} else {flag=1}} flag==1 {print $0 >> "YLZTemp.dis"}' symbol.s.sym app.s.dis
  awk  '{{gsub($1 ,$1":", $1)}; print > "app.s.dis"}' YLZTemp.dis    # add ":" 
  awk  '{print $1 "    " $2 $3 $4 $5 "    " $6 " " $7 " " $8 " " $9 " " $10 " " $11 " " $12 " " $13 " " $14 " " $15 " " $16 " " $17 " " $18 " " $19 " " $20 " "$21 " " $22 " " $23 " " $24 " " $25 " " $26 " " $27 " " $28 " " $29 " " $30 " "> "YLZTemp.dis"}'  app.s.dis
#  comment the way , the output disassemble file has too many space signal
#  awk  '{gsub($1, $1"    ", $1)}; print > "YLZTemp.dis" {gsub($5,$5"    ", $5)} NR>5 {for(i=6; i<=NF; i++) gsub($i, $i" ", $i)}; print $0 > "YLZTemp.dis"}' app.s.dis
  mv YLZTemp.dis app.s.dis

else
  echo "file::PostProcessDis.sh--There is no object file of Scalar Processing Unit(SPU)"
fi
