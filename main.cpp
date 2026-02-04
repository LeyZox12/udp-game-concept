#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <PointEngine.h>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <format>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <system_error>
#include <thread>
#include "OnUpdateContext.hpp"
#include "Player.hpp"
#include "Rectangle.h"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Network/Socket.hpp"
#include "SFML/Window/Mouse.hpp"
#include "operators.hpp"
#include "globals.hpp"
#include <memory>	

using namespace sf;
using namespace std;

string SERVER_STR = "-s";
string CLIENT_STR = "-c";

void runServer(char* argv[]);
void runClient(char* argv[]);

//TODO load map automatic + Collisions + gravity + random weapons + rounds system
//TODO FIX POINT NOT SENDING RIGHT




struct Map
{
	
};


int main(int argc, char* argv[])
{
	if(argc < 2) return -1;

	if((string)argv[1] == SERVER_STR)
		runServer(argv);
	else
		runClient(argv);

	return 0;
}

void displayMap(RenderWindow& window, vector<vector<int>>& map)
{
	
	RectangleShape rect({BLOCK_SIZE, BLOCK_SIZE});
	for(int i = 0; i < map.size(); i++)
	{
		for(int j = 0; j < map[i].size(); j++)
		{
			int block = map[i][j];
			Color col = Color::Black;
			switch(block)
			{
				case(HARD):
					col = Color::White;
					break;
				case(PLATFORM):
					col = Color(128, 128, 128);
					break;
				case(DEATH):
					col = Color::Red;
					break;
			}
			if(col != Color::Black)
			{
				rect.setFillColor(col);
				rect.setPosition(Vector2f(j * BLOCK_SIZE, i * BLOCK_SIZE));
				window.draw(rect);
			}
		}
	}
}

void runServer(char* argv[])
{

	int playerCount = 0;
	
	UdpSocket UDP;
	TcpListener TCP;

	vector<Player> players;
	vector<thread> threads;
	vector<unique_ptr<PointEngine>> pointEngines; 
	vector<TcpSocket> playersTCP;
	vector<IpAddress> ips;
	vector<unsigned short> ports;

	mutex posMutex;
	atomic<bool> running{true};

	

	unsigned short port = stoi(argv[2]);

	if(UDP.bind(Socket::AnyPort) != Socket::Status::Done ||
	  TCP.listen(port) != Socket::Status::Done)
	{
		cout << "Failed to start server, closing program" << endl; 
		return;
	}
	UDP.setBlocking(false);
	std::cout << "port:" << TCP.getLocalPort() << std::endl;
	while(playerCount < 2)
	{
		playersTCP.emplace_back(TcpSocket());
		if(TCP.accept(playersTCP.back()) == Socket::Status::Done)
		{
			pointEngines.push_back(make_unique<PointEngine>());
			ips.emplace_back(playersTCP.back().getRemoteAddress().value());
			cout << "New player Joined " << playersTCP.back().getRemotePort();
			Packet udpPacket;
			players.emplace_back(Player(*pointEngines[playerCount], Vector2f(100, 0)));
			udpPacket << UDP_PORT << UDP.getLocalPort() << playerCount << 2;
			if(playersTCP.back().send(udpPacket) != Socket::Status::Done){
				cout << "Couldn't receive player correctly, closing program";
			}
			playerCount++;

		}
		Packet p;
		if(playersTCP.back().receive(p) == Socket::Status::Done)
		{
			int type;
			unsigned short port;
			p >> type >> port;
			if(type == CLIENT_IN)
				ports.emplace_back(port);
		}
	}
	
	Packet p;
	p << MAP_TRANSFER_INFO << "res/Maps/0_0.png";
	for(auto& player: playersTCP)
	{
		if(player.send(p) == Socket::Status::Done)
			cout << "Map info sent successfully" << endl;
		else
			cout << "Failed to send map info" << endl;
	}
	
	RenderWindow window(VideoMode({512, 512}), "Server");

		while(running)
		{
			while(optional<Event> e = window.pollEvent())
			{
				if(e->is<Event::Closed>())
				{
					running = false;
					UDP.unbind();
					TCP.close();
					window.close();
					return;
				}
			}
			for(int i = 0; i < playerCount; i++){
				Packet p;
				optional<IpAddress> address = playersTCP[i].getRemoteAddress();
				unsigned short port = playersTCP[i].getRemotePort();
				while(UDP.receive(p, address, port) == Socket::Status::Done)
				{
					int type, count, key, index;
					p >> type;
					if(type == PACKET_TYPE::CLIENT_MOVE)
					{
                        p >> index;
						p >> players[index];
					}
					p.clear();

				}

				Packet positionsPacket;
				positionsPacket << CLIENT_POSITIONS;
				for(int j = 0; j < playerCount; j++)
				{
					positionsPacket << players[j];
				}
				UDP.send(positionsPacket, ips[i], ports[i]);
			}
			this_thread::sleep_for(chrono::milliseconds(1));
		}
		UDP.unbind();

}

void runClient(char* argv[])
{
	RenderWindow window(VideoMode({512, 512}), "GambleCore");

	PointEngine pe;
	UdpSocket clientUDP;
	TcpSocket clientTCP;

	vector<Player> players; 
	optional<IpAddress> ipToResolve = IpAddress::resolve((string) argv[2]);

	vector<vector<int>> map;
	mutex mapMtx;

	clientUDP.setBlocking(false);

	if(!ipToResolve.has_value())
	{
		cout << "entered ip address is invalid, closing program" << endl;
		return;
	}
	
	IpAddress serverIp = ipToResolve.value();
	unsigned short tcpPort = stoi(argv[3]);
	unsigned short udpPort;
	unsigned int playerCount = 0;
	unsigned int pId = 0;


	if(clientTCP.connect(serverIp, tcpPort) != Socket::Status::Done)
	{
		cout << "Couldn't connect to server, closing program" << endl;
		return;
	}

	if(clientUDP.bind(Socket::AnyPort) != Socket::Status::Done){
		cout << "No ports available, closing program" << endl;
		return;
	}
	else
	{
		cout << "Listening to server on port: " << clientUDP.getLocalPort() << endl;
	}

	cout << "Connecting to server..." << endl;

	Packet p;
	p << CLIENT_IN << clientUDP.getLocalPort();

	if(clientTCP.send(p) != Socket::Status::Done)
	{
		cout << "Couldn't reach server, closing program";
		return;
	}
	else
		cout << "Connected Successfully" << endl;

	Packet pingPacket;
	if(clientTCP.receive(pingPacket) == Socket::Status::Done)
	{
		int type;
		pingPacket >> type;
		if(type == UDP_PORT)
		{
			pingPacket >> udpPort >> pId >> playerCount;
            cout <<"playercount:"<< playerCount << endl;
            for(int i = 0; i < playerCount; i++)
            {
                players.emplace_back(Player(pe, Vector2f(100, 0)));
            }
            window.setPosition(Vector2i(pId * 512, 0));
		}
		cout << "Recieved udp port from server:" << udpPort << endl;
	}

	atomic<bool> running{true};
	mutex keyMutex;
	mutex posMutex;

	CircleShape sprite(PLAYER_RADIUS);
	sprite.setOrigin({sprite.getRadius(), sprite.getRadius()});
	vector<Keyboard::Key> heldKeys;

	thread sendUpdate = thread([&running, &heldKeys, &clientUDP, &keyMutex, &serverIp, &udpPort, &pId, &players](){
		auto end = chrono::steady_clock::now();
		while(running)
		{
			Packet p;
			vector<Keyboard::Key> buffer;
			{
				lock_guard<mutex> lock(keyMutex);
				buffer = heldKeys;
			}
				p << PACKET_TYPE::CLIENT_MOVE;
				p << pId;
				p << players[pId];

				if(clientUDP.send(p, serverIp, udpPort) != Socket::Status::Done) 
					cout << "failed to send keyboard data to server" << endl;
			auto now = chrono::steady_clock::now();
			end += chrono::milliseconds(33);
			if(now > end)
				end = now + chrono::milliseconds(33);
			this_thread::sleep_until(end);
		}
	});
	thread getTCPUpdate = thread([&running, &clientTCP, &map, &mapMtx, &pe, &players, &pId](){
		while(running)
		{
			Packet packet;
			while(clientTCP.receive(packet) == Socket::Status::Done)
			{
				int type;
				packet >> type;
				switch(type){
					case(MAP_TRANSFER_INFO):
						lock_guard<mutex> lock(mapMtx);
						string mapName = "";
						packet >> mapName;
						Image sprite;
						sprite.loadFromFile(mapName);
						map = vector<vector<int>>(sprite.getSize().x, vector<int>(sprite.getSize().y, EMPTY));
						for(int i = 0; i < sprite.getSize().y; i++)
						{
							for(int j = 0; j < sprite.getSize().x; j++)
							{
								
								Color col = sprite.getPixel(Vector2u(j, i));
								int block = EMPTY;
								if(col == Color::White)
								{
									block = HARD;
									pe.addRectangle(IntRect({j * BLOCK_SIZE, i*BLOCK_SIZE},{BLOCK_SIZE, BLOCK_SIZE}));
								}
								else if(col == Color(255, 0, 0)) block = DEATH;
								else if(col == Color(0, 255, 0))
								{
                                    static int spawnId = 0;
									block = SPAWN;
                                    if(spawnId++ == pId){
                                        players[pId].setPos(vec2(j * BLOCK_SIZE, i * BLOCK_SIZE - pId * PLAYER_RADIUS * 3));
                                        players[pId].setOldPos(players[pId].getPos());
                                    }
								}
								else if(col == Color(128, 128, 128)) block = PLATFORM;
								cout << block;
								map[i][j] = block;
							}
							cout << endl;
						}
						cout << "Map loaded successfully" << endl;
					break;
				}
			}
		}
		this_thread::sleep_for(chrono::milliseconds(1));
	});
	thread getUpdate = thread([&running, &players, &clientUDP, &posMutex, serverIp, udpPort, playerCount, &pId, &pe](){
		optional<IpAddress> adress = serverIp;
		unsigned short port = udpPort;
		while(running)
		{
			for(auto& player: players)
			{
			}
			Packet positionsPacket;
			while(clientUDP.receive(positionsPacket, adress, port) == Socket::Status::Done)
			{
				lock_guard<mutex> lock(posMutex);
				int type;
				positionsPacket >> type;
				switch(type){
					case(CLIENT_POSITIONS):
						for(int i = 0; i < playerCount; i++)
						{
							if(i != pId)
								positionsPacket >> players[i];
							else
							{
								float dummy;
								positionsPacket 
								>> dummy 
								>> dummy 
								>> dummy 
								>> dummy;
							}
						}
					break;
				}
				positionsPacket.clear();
			}
		}
		this_thread::sleep_for(chrono::milliseconds(1));
	});

	Player* thisPlayer = &players[pId];
	sf::Clock deltaClock;
	float dt = 0.016;
	window.setFramerateLimit(60);
	while(window.isOpen())
	{
		while(optional<Event> e = window.pollEvent())
		{
			if(e->is<Event::Closed>())
			{
				window.close();
				running = false;
			}
			if(e->is<Event::KeyPressed>())
			{
				Keyboard::Key k = e->getIf<Event::KeyPressed>()->code;
				{
					lock_guard<mutex> lock(keyMutex);
					if(count(heldKeys.begin(), heldKeys.end(), k) == 0) heldKeys.push_back(k);
				}
			}
			else if(e->is<Event::KeyReleased>())
			{
				Keyboard::Key k = e->getIf<Event::KeyReleased>()->code;
				{
					lock_guard<mutex> lock(keyMutex);
					if(count(heldKeys.begin(), heldKeys.end(), k) == 1) 
						heldKeys.erase(find(heldKeys.begin(), heldKeys.end(), k));
				}
			}
		}
		Vector2f diff = (Vector2f)Mouse::getPosition(window) - players[pId].getPos();

		for(auto& key: heldKeys)
		{
			switch(key)
			{
				case(Keyboard::Key::Z):
					break;
				case(Keyboard::Key::Q):
					players[pId].setPos(players[pId].getPos() + vec2(-PLAYER_SPEED * dt, 0));
					break;
				case(Keyboard::Key::S):
					break;
				case(Keyboard::Key::D):
					players[pId].setPos(players[pId].getPos() + vec2(PLAYER_SPEED * dt, 0));
					break;
				default:
				break;

			}
		}
		
		pe.updatePointPos(dt, (vec2)Mouse::getPosition(window));
		pe.applyCollisions(4);
		pe.applyConstraints(4, dt);

		players[pId].camera.setCenter(players[pId].getPos());

		window.clear(Color::Black);
		window.setView(players[pId].camera);

		{
			lock_guard<mutex> lock(mapMtx);
			//displayMap(window, map);
		}
		for(int i = 0; i < players.size(); i++)
		{
			sprite.setPosition(players[i].getPos());
			sprite.setRotation(radians(players[i].rot));
			window.draw(sprite);
		}
		pe.displayAsRects(window, Color::White, 5.f);

		window.display();
		dt = deltaClock.restart().asSeconds() * 1.1;
		window.setTitle(format("{:}", 1.f / dt));
	}
	clientUDP.unbind();
}