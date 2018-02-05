./atpg -tdfatpg -ndet 8 ../sample_circuits/adder.ckt > ../tdf_patterns/adder.pat
./atpg -ndet 8 -tdfsim ../tdf_patterns/adder.pat ../sample_circuits/adder.ckt

./atpg -tdfatpg -ndet 8 ../sample_circuits/bar.ckt > ../tdf_patterns/bar.pat
./atpg -ndet 8 -tdfsim ../tdf_patterns/bar.pat ../sample_circuits/bar.ckt


./atpg -tdfatpg -ndet 8 ../sample_circuits/dec.ckt > ../tdf_patterns/dec.pat
./atpg -ndet 8 -tdfsim ../tdf_patterns/dec.pat ../sample_circuits/dec.ckt


./atpg -tdfatpg -ndet 8 ../sample_circuits/multi.ckt > ../tdf_patterns/multi.pat
./atpg -ndet 8 -tdfsim ../tdf_patterns/multi.pat ../sample_circuits/multi.ckt

./atpg -tdfatpg -ndet 8 ../sample_circuits/sin.ckt > ../tdf_patterns/sin.pat
./atpg -ndet 8 -tdfsim ../tdf_patterns/sin.pat ../sample_circuits/sin.ckt

./atpg -tdfatpg -ndet 8 ../sample_circuits/maxx.ckt > ../tdf_patterns/maxx.pat
./atpg -ndet 8 -tdfsim ../tdf_patterns/maxx.pat ../sample_circuits/maxx.ckt



./atpg -tdfatpg -compression -ndet 8 ../sample_circuits/adder.ckt > ../tdf_patterns/adder.pat
./atpg -ndet 8 -tdfsim ../tdf_patterns/adder.pat ../sample_circuits/adder.ckt

./atpg -tdfatpg -compression -ndet 8 ../sample_circuits/bar.ckt > ../tdf_patterns/bar.pat
./atpg -ndet 8 -tdfsim ../tdf_patterns/bar.pat ../sample_circuits/bar.ckt


./atpg -tdfatpg -compression -ndet 8 ../sample_circuits/dec.ckt > ../tdf_patterns/dec.pat
./atpg -ndet 8 -tdfsim ../tdf_patterns/dec.pat ../sample_circuits/dec.ckt


./atpg -tdfatpg -compression -ndet 8 ../sample_circuits/multi.ckt > ../tdf_patterns/multi.pat
./atpg -ndet 8 -tdfsim ../tdf_patterns/multi.pat ../sample_circuits/multi.ckt

./atpg -tdfatpg -compression -ndet 8 ../sample_circuits/sin.ckt > ../tdf_patterns/sin.pat
./atpg -ndet 8 -tdfsim ../tdf_patterns/sin.pat ../sample_circuits/sin.ckt

./atpg -tdfatpg -compression -ndet 8 ../sample_circuits/maxx.ckt > ../tdf_patterns/maxx.pat
./atpg -ndet 8 -tdfsim ../tdf_patterns/maxx.pat ../sample_circuits/maxx.ckt
