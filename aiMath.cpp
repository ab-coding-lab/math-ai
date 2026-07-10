#include <iostream>
#include <cmath>
// eulers number func
double e() {
    return 2.71828 ;
}
// activation function 1
class Activation {
    public:
    int relu(float input) {
        float relu_val {input} ;
        float output_relu ;
        if (relu_val > 0) {
            output_relu = input ;
        }
        else {
            output_relu = 0;
        }
        return output_relu ;
        }
    double sig(double x) {
        return 1 / (1 + pow(e(), (x * -1))) ;
    }
    double tanh(double input) {
        double negative { 0 - input } ;
        double output { ( pow( e() , input) -  pow( e() , negative ) ) / ( pow( e() , input ) +  pow( e() , negative ) ) } ;
        return output ;
    }
    int dv_relu(float input) {
        if (relu(input) > 0) {
            return 1 ;
        }
        else {
            return 0 ;
        }
    }
    float dv_tanh(float input) {
        return 1 -  pow(tanh(input), 2) ;
    }
};
class Summation {
        int addSummation(int x) {
                if (x == 1) {
                        return x ;
                }
                return x + addSummation(x - 1) ;
        }
};
