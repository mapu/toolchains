// RUN: clang -target mspu -S -o %t.s %s

double f( double r){
    return -r;
}

double g( double r, double s){
    return r/s;
}

