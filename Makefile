
CXXFLAGS = -std=c++23 -g -I"../../../SFML-3.0.0/include" -I"../../class/PointEngine/include" -Iinclude

final : main.o 
	g++ $(CXXFLAGS) main.o -o UDP -L"../../../SFML-3.0.0/lib" -L"../../class/PointEngine/lib" -lsfml-window-d -lsfml-graphics-d -lsfml-network-d -lsfml-system-d -lPointEngine

main.o : main.cpp
	g++ $(CXXFLAGS) main.cpp -c
