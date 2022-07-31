struct simple_struct {
	int a;
	int b;
	int c;
};

int main() {
	struct simple_struct instance = {1, 2, 3};
	return instance.a + instance.b + instance.c;
}