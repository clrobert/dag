run:
	clang++ -std=c++11 -stdlib=libc++ resource_manager.cpp
	./a.out

explain:
	clang++ -std=c++11 -stdlib=libc++ -Weverything resource_manager.cpp
	./a.out
