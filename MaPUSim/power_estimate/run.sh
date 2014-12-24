cd McPAT08release
make
cd ..
perl m5-mcpat.pl ../m5out/stats.txt ../m5out/config.ini mcpat-template.xml > result.xml
./McPAT08release/mcpat -infile result.xml

