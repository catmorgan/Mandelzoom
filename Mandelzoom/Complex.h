class Complex {
public:
    double x;
	double y;

		Complex (double x_init, double y_init) :
		x (x_init), y (y_init) {};

		Complex operator+ (Complex that) {
			float nx = x + that.x; 
			float ny = y + that.y;
			return Complex (nx,ny);
		}

		Complex operator* (Complex that) {
			float nx = (x * that.x) - (y * that.y);
			float ny = (x * that.y) + (y * that.x);
			return Complex (nx,ny);
		}
};

