#include <thread>
#define ZMPRINTER_IMPL
#include "zmprinter.hpp"

struct vec3 {
	float x,y,z;
};

std::ostream &operator <<(std::ostream &os,const vec3 v) {
	os << '(' << v.x << ',' << v.y<< ',' << v.z << ')';
	return os;
}
int main (int argc, char *argv[]) {
	vec3 v1 = {1,2,3};	
	vec3 v2 = {4,5,6};	
	zm::printer mprint("time {%t} {%m}\n");
	mprint.print("this {%1} , {%0}", v1, v2);
	mprint.print("this message {%1}, {%2}, {%3}", 1, 2, 3);
	return 0;
}
