#include <iostream>
#include <cstdlib>
#include <vector>
#include <memory>
#include <ctime>
#include <algorithm>

//defining standard variables
#define goal 700
#define pirateLife 100
#define merchandLife 100
#define repairLife 100
#define explorerLife 50 //explorer ships are taking quite less damage than the other ships due to their function, so they start with 50 life points
#define ports 10
#define treasures 20

using std::cout;
using std::cin;
using std::endl;
using namespace std;

class Environment
{
private:
	int weather;
	bool treasure;
	bool port;
	bool ship;
public:
	Environment()
		:port(false), treasure(false), ship(false){};
	void setWeatherCond(int x){weather = x;}
	void setTreasurePos(bool x){treasure = x;}
	void setPortPos(bool x){port = x;}
	void setShipPos(bool x){ship = x;}
	int getWeatherCond(){return weather;}
	bool getTreasurePos(){return treasure;}
	bool getPortPos(){return port;}
	bool getShipPos(){return ship;}
};

class Ship{
protected:
	int type;
	int x;
	int y;
	int maxLife;
	int curLife;
	int treasure;
	int speed;
	static int totalNum;
	int shipNum;
public:
	Ship()
		:x(0), y(0){
		totalNum++;
	}
	virtual void Function(vector<std::unique_ptr<Ship>> & v, vector<vector<Environment>> & map, int i){}
	virtual void setMaxLife(int) = 0;
	virtual void setCurLife(int) = 0;
	virtual void setSpeed(int) = 0;
	virtual void setTreasure(int) = 0;
	virtual void setX(int) = 0;
	virtual void setY(int) = 0;
	virtual void Move(vector<std::unique_ptr<Ship>> & v, vector<vector<Environment>> & map, int i){}
	virtual int getMaxLife(void) = 0;
	virtual int getCurLife(void) = 0;
	virtual int getSpeed(void) = 0;
	virtual int getTreasure(void) = 0;
	virtual int getType(void) = 0;
	virtual int getX(void) = 0;
	virtual int getY(void) = 0;
	virtual int getSnum(void) = 0;
	int getNum(){ return totalNum; }
};

class PirateShip : public Ship{
private:
	static int pirNum;
	int damage;
public:
	PirateShip(){
		pirNum++;
		shipNum = pirNum;
		type = 1;
		speed = 1;
		treasure = 50;
		damage = 10;
		maxLife = pirateLife;
		curLife = maxLife;
	}
	void Function(vector<std::unique_ptr<Ship>> & v, vector<vector<Environment>> & map, int i){
		int x = v[i]->getX();
		int y = v[i]->getY();
		for (int u = -1; u < 2; u++){
			for (int w = -1; w < 2; w++){
				if ((x + u >= 0) && (x + u <= 9) && (y + w >= 0) && (y + w <= 9)){
					if ((map[x + u][y + w].getShipPos() == true) && ((u != 0) || (w != 0))){
						int k = 0;
						for (vector<std::unique_ptr<Ship>>::iterator j = v.begin(); j != v.end();){
							if ((v[k]->getX() == x + u) && (v[k]->getY() == y + w) && (v[k]->getType() != 1)){//pirate ships do not attack other pirate ships
								v[k]->setCurLife(v[k]->getCurLife() - 10);
								v[k]->setTreasure(v[k]->getTreasure() - 20);
								v[i]->setTreasure(v[i]->getTreasure() + 20);
								char* sh = "";
								switch (v[k]->getType()){
								case 1:
									sh = "Pirate Ship ";
									break;
								case 2:
									sh = "Merchand Ship ";
									break;
								case 3:
									sh = "Repair Ship ";
									break;
								case 4:
									sh = "Explorer Ship ";
									break;
								}
								cout << "Pirate Ship " << v[i]->getSnum() << " attacks " << sh << v[k]->getSnum() << " and steals 20 treasure.\n";
							}
							j++;
							k++;
						}
					}
				}
				else continue;
			}
		}
	}
	void setMaxLife(int x){ maxLife = x; }
	void setCurLife(int x){ curLife = x; }
	void setSpeed(int x){ speed = x; }
	void setTreasure(int x){ treasure = x; }
	void setX(int a){ x = a; }
	void setY(int a){ y = a; }
	void Move(vector<std::unique_ptr<Ship>> & v, vector<vector<Environment>> & map, int i){
		int r = 0;//pirate ship, moves with speed=1, every other ship moves with speed=2 except the explorer ships that have speed=3(including their function).
		do{
			int x;
			int y;
			x = rand() % 3;
			y = rand() % 3;
			int x1 = v[i]->getX();
			int y1 = v[i]->getY();
			if ((x1 + x - 1 >= 0) && (x1 + x - 1 <= 9) && (y1 + y - 1 >= 0) && (y1 + y - 1 <= 9) && ((x != 1) || (y != 1)) && (map[x1 + x - 1][y1 + y - 1].getShipPos() == false) && (map[x1 + x - 1][y1 + y - 1].getPortPos() == false)) {
				map[x1][y1].setShipPos(false);
				v[i]->setX(x1 + x - 1);
				v[i]->setY(y1 + y - 1);
				map[x1 + x - 1][y1 + y - 1].setShipPos(true);
				r = 1;
			}
			else continue;
		} while (r == 0);
	}
	int getSnum(){ return shipNum; }
	int getMaxLife(){ return maxLife; }
	int getCurLife(){ return curLife; }
	int getSpeed(){ return speed; }
	int getTreasure(){ return treasure; }
	int getType(){ return type; }
	int getX(){ return x; }
	int getY(){ return y; }
};

class MerchandShip : public Ship{
private:
	static int merNum;
public:
	MerchandShip(){
		merNum++;
		shipNum = merNum;
		type = 2;
		speed = 2;
		treasure = 100;
		maxLife = merchandLife;
		curLife = maxLife;
	}
	void Function(vector<std::unique_ptr<Ship>> & v, vector<vector<Environment>> & map, int i){
		int x = v[i]->getX();
		int y = v[i]->getY();
		int q = 0;//using q to stop a merchand ship from visiting 2 ports in one round, which is unfair for the other ships
		for (int u = -1; u < 2; u++){
			for (int w = -1; w < 2; w++){
				if ((x + u >= 0) && (x + u <= 9) && (y + w >= 0) && (y + w <= 9)){
					if ((map[x + u][y + w].getPortPos() == true) && (q == 0)){
						v[i]->setTreasure(v[i]->getTreasure() + 10);
						cout << "Merchand Ship " << v[i]->getSnum() << " reaches Port on coordinates " << x + u + 1 << ":" << y + w + 1 << " and increases its treasure by 10.\n";
						q = 1;
					}
				}
			}
		}
	}
	void setMaxLife(int x){ maxLife = x; }
	void setCurLife(int x){ curLife = x; }
	void setSpeed(int x){ speed = x; }
	void setTreasure(int x){ treasure = x; }
	void setX(int a){ x = a; }
	void setY(int a){ y = a; }
	void Move(vector<std::unique_ptr<Ship>> & v, vector<vector<Environment>> & map, int i){
		for (int k = 0; k < 2; k++){
			int r = 0;
			do{
				int x;
				int y;
				x = rand() % 3;
				y = rand() % 3;
				int x1 = v[i]->getX();
				int y1 = v[i]->getY();
				if ((x1 + x - 1 >= 0) && (x1 + x - 1 <= 9) && (y1 + y - 1 >= 0) && (y1 + y - 1 <= 9) && ((x != 1) || (y != 1)) && (map[x1 + x - 1][y1 + y - 1].getShipPos() == false) && (map[x1 + x - 1][y1 + y - 1].getPortPos() == false)) {
					map[x1][y1].setShipPos(false);
					v[i]->setX(x1 + x - 1);
					v[i]->setY(y1 + y - 1);
					map[x1 + x - 1][y1 + y - 1].setShipPos(true);
					r = 1;
				}
				else continue;
			} while (r == 0);
		}
	}
	int getSnum(){ return shipNum; }
	int getMaxLife(){ return maxLife; }
	int getCurLife(){ return curLife; }
	int getSpeed(){ return speed; }
	int getTreasure(){ return treasure; }
	int getType(){ return type; }
	int getX(){ return x; }
	int getY(){ return y; }
};

class RepairShip : public Ship{
private:
	static int repNum;
public:
	RepairShip(){
		repNum++;
		shipNum = repNum;
		type = 3;
		speed = 2;
		treasure = 20;
		maxLife = repairLife;
		curLife = maxLife;
	}
	void Function(vector<std::unique_ptr<Ship>> & v, vector<vector<Environment>> & map, int i){
		int x1 = v[i]->getX();
		int y1 = v[i]->getY();
		for (int u = -1; u < 2; u++){//repair ships will repair all other ships except pirate ships.
			for (int w = -1; w < 2; w++){//they will also check if the other ship has the required treasure to pay them.
				if ((x1 + u >= 0) && (x1 + u <= 9) && (y1 + w >= 0) && (y1 + w <= 9)){
					if ((map[x1 + u][y1 + w].getShipPos() == true) && ((u != 0) || (w != 0))){
						int k = 0;
						for (vector<std::unique_ptr<Ship>>::iterator j = v.begin(); j != v.end();){
							if ((v[k]->getX() == x + u) && (v[k]->getY() == y + w) && (v[k]->getCurLife() + 4 < v[k]->getMaxLife()) && (v[k]->getTreasure() > 10) && (v[k]->getType() != 1)){
								v[k]->setCurLife(v[k]->getCurLife() + 5);
								v[k]->setTreasure(v[k]->getTreasure() - 10);
								v[i]->setTreasure(v[i]->getTreasure() + 10);
								char* sh = "";
								switch (v[k]->getType()){
								case 1:
									sh = "Pirate Ship ";
									break;
								case 2:
									sh = "Merchand Ship ";
									break;
								case 3:
									sh = "Repair Ship ";
									break;
								case 4:
									sh = "Explorer Ship ";
									break;
								}
								cout << "Repair Ship " << v[i]->getSnum() << " repaired " << sh << v[k]->getSnum() << " and gained 10 treasure\n";
							}
							j++;
							k++;
						}
					}
				}
			}
		}
	}
	void setMaxLife(int x){ maxLife = x; }
	void setCurLife(int x){ curLife = x; }
	void setSpeed(int x){ speed = x; }
	void setTreasure(int x){ treasure = x; }
	void setX(int a){ x = a; }
	void setY(int a){ y = a; }
	void Move(vector<std::unique_ptr<Ship>> & v, vector<vector<Environment>> & map, int i){
		for (int k = 0; k < 2; k++){
			int r = 0;
			do{
				int x;
				int y;
				x = rand() % 3;
				y = rand() % 3;
				int x1 = v[i]->getX();
				int y1 = v[i]->getY();
				if ((x1 + x - 1 >= 0) && (x1 + x - 1 <= 9) && (y1 + y - 1 >= 0) && (y1 + y - 1 <= 9) && ((x != 1) || (y != 1)) && (map[x1 + x - 1][y1 + y - 1].getShipPos() == false) && (map[x1 + x - 1][y1 + y - 1].getPortPos() == false)) {
					map[x1][y1].setShipPos(false);
					v[i]->setX(x1 + x - 1);
					v[i]->setY(y1 + y - 1);
					map[x1 + x - 1][y1 + y - 1].setShipPos(true);
					r = 1;
				}
				else continue;
			} while (r == 0);
		}
	}
	int getSnum(){ return shipNum; }
	int getMaxLife(){ return maxLife; }
	int getCurLife(){ return curLife; }
	int getSpeed(){ return speed; }
	int getTreasure(){ return treasure; }
	int getType(){ return type; }
	int getX(){ return x; }
	int getY(){ return y; }
};

class ExplorerShip : public Ship{
private:
	static int exNum;
public:
	ExplorerShip(){
		exNum++;
		shipNum = exNum;
		type = 4;
		speed = 2;
		treasure = 20;
		maxLife = explorerLife;
		curLife = maxLife;
	}
	void Function(vector<std::unique_ptr<Ship>> & v, vector<vector<Environment>> & map, int i){//sometimes an explorer ship might move close to a pirate ship while avoiding extreme weather conditions
		int x1 = v[i]->getX();
		int y1 = v[i]->getY();
		int f = 0;
		for (int u = -1; u < 2; u++){//priority: the explorer ship will try to avoid pirate ships first, since they do more damage than weather
			for (int w = -1; w < 2; w++){
				if ((x1 + u >= 0) && (x1 + u <= 9) && (y1 + w >= 0) && (y1 + w <= 9)){
					if (map[x1 + u][y1 + w].getShipPos() == true){
						int k = 0;
						for (vector<std::unique_ptr<Ship>>::iterator j = v.begin(); j != v.end();){
							if ((v[k]->getX() == x1 + u) && (v[k]->getY() == y1 + w) && (v[k]->getType() == 1)){
								if ((x1 - u <= 9) && (x1 - u >= 0) && (y1 - w <= 9) && (y1 - w >= 0)){
									map[x1][y1].setShipPos(false);
									v[i]->setX(x1 - u);
									v[i]->setY(y1 - w);
									map[x1 - u][y1 - w].setShipPos(true);
									cout << "Explorer Ship " << v[i]->getSnum() << " moved to " << x1 - u + 1 << ":" << y1 - w + 1 << " to avoid Pirate Ship " << v[k]->getSnum() << endl;
									f = 1;
									break;
								}
								else cout << "Explorer Ship " << v[i]->getSnum() << " failed to avoid Pirate Ship "<< v[k]->getSnum() << endl;
							}
							k++;
							j++;
						}
					}
				}
			}
		}
		if (f == 0){//f is used to stop the explorer ships from avoiding weather if they are near a pirate ship
			int min = map[x1][y1].getWeatherCond();
			int minx = 0;
			int miny = 0;
			for (int u = -1; u < 2; u++){
				for (int w = -1; w < 2; w++){
					if ((x1 + u >= 0) && (x1 + u <= 9) && (y1 + w >= 0) && (y1 + w <= 9)){
						if (map[x1 + u][y1 + w].getWeatherCond() < min){
							min = map[x1 + u][y1 + w].getWeatherCond();
							minx = x1 + u;
							miny = y1 + w;
						}
					}
				}
			}
			if (min != map[x1][y1].getWeatherCond()){
				map[x1][y1].setShipPos(false);
				v[i]->setX(minx);
				v[i]->setY(miny);
				map[minx][miny].setShipPos(true);
				cout << "Explorer Ship " << v[i]->getSnum() << " moved to " << minx + 1 << ":" << miny + 1 << " to avoid extreme weather conditions.\n";
			}
		}

	}
	void setMaxLife(int x){ maxLife = x; }
	void setCurLife(int x){ curLife = x; }
	void setSpeed(int x){ speed = x; }
	void setTreasure(int x){ treasure = x; }
	void setX(int a){ x = a; }
	void setY(int a){ y = a; }
	void Move(vector<std::unique_ptr<Ship>> & v, vector<vector<Environment>> & map, int i){
		for (int k = 0; k < 2; k++){
			int r = 0;
			do{
				int x;
				int y;
				x = rand() % 3;
				y = rand() % 3;
				int x1 = v[i]->getX();
				int y1 = v[i]->getY();
				if ((x1 + x - 1 >= 0) && (x1 + x - 1 <= 9) && (y1 + y - 1 >= 0) && (y1 + y - 1 <= 9) && ((x != 1) || (y != 1)) && (map[x1 + x - 1][y1 + y - 1].getShipPos() == false) && (map[x1 + x - 1][y1 + y - 1].getPortPos() == false)) {
					map[x1][y1].setShipPos(false);
					v[i]->setX(x1 + x - 1);
					v[i]->setY(y1 + y - 1);
					map[x1 + x - 1][y1 + y - 1].setShipPos(true);
					r = 1;
				}
				else continue;
			} while (r == 0);
		}
	}
	int getSnum(){ return shipNum; }
	int getMaxLife(){ return maxLife; }
	int getCurLife(){ return curLife; }
	int getSpeed(){ return speed; }
	int getTreasure(){ return treasure; }
	int getType(){ return type; }
	int getX(){ return x; }
	int getY(){ return y; }
};
//initializing static variables
int Ship::totalNum = 0;
int PirateShip::pirNum = 0;
int MerchandShip::merNum = 0;
int RepairShip::repNum = 0;
int ExplorerShip::exNum = 0;

//the two sorting options:
bool srt(std::unique_ptr<Ship> & t1, std::unique_ptr<Ship> & t2){ return t1->getTreasure() > t2->getTreasure(); }
bool srt2(std::unique_ptr<Ship> & t1, std::unique_ptr<Ship> & t2){
	if (t1->getType() != t2->getType()) return (t1->getType() < t2->getType());
	else return (t1->getSnum() < t2->getSnum());
}

int main()
{
	srand(time(0));
	std::vector<vector<Environment>> map(10, vector<Environment>(10));
	std::vector<std::unique_ptr<Ship>>v;
	//inserting the basic 8 ships of the simulation
	v.emplace_back(new PirateShip);
	v.emplace_back(new PirateShip);
	v.emplace_back(new MerchandShip);
	v.emplace_back(new MerchandShip);
	v.emplace_back(new RepairShip);
	v.emplace_back(new RepairShip);
	v.emplace_back(new ExplorerShip);
	v.emplace_back(new ExplorerShip);
	int i;
	int t;
	int num1 = 0;
	int num2 = 0;
	for (num1 = 0; num1 < 10; num1++){
		for (num2 = 0; num2 < 10; num2++){
			map[num1][num2].setWeatherCond(1 + rand() % 10);//initializing weather conditions
		}
	}
	num1 = 0;
	num2 = 0;
	while (num1 <= ports){
		int x = rand() % 10;
		int y = rand() % 10;
		if ((map[x][y].getPortPos() == false) && (map[x][y].getTreasurePos() == false))
		{
			map[x][y].setPortPos(true);//setting random ports on the map(10)
			num1++;
		}
	}
	while (num2 <= treasures){
		int x = rand() % 10;
		int y = rand() % 10;
		if ((map[x][y].getPortPos() == false) && (map[x][y].getTreasurePos() == false))
		{
			map[x][y].setTreasurePos(true);//setting random treasure points on the map(20)
			num2++;
		}
	}
	int num3 = 0;
	for (vector<std::unique_ptr<Ship>>::iterator j = v.begin(); j != v.end();){//random positioning of the ships on the map
		int x = rand() % 10;
		int y = rand() % 10;
		if ((map[x][y].getPortPos() == false) && (map[x][y].getShipPos() == false)){
			v[num3]->setX(x);
			v[num3]->setY(y);
			map[x][y].setShipPos(true);
			j++;
			num3++;
		}
	}
	i = 0;
	for (vector<std::unique_ptr<Ship>>::iterator j = v.begin(); j != v.end();){
		switch (v[i]->getType()){
		case 1:
			cout << "Pirate Ship ";
			break;
		case 2:
			cout << "Merchand Ship ";
			break;
		case 3:
			cout << "Repair Ship ";
			break;
		case 4:
			cout << "Explorer Ship ";
			break;
		}
		cout << v[i]->getSnum() << " initial coordinates are " << v[i]->getX() + 1 << ":" << v[i]->getY() + 1 << endl;//displaying the initial coordinates of the basic ships...
		i++;
		j++;
	}
	cout << endl;
	for (i = 0; i<10; i++)
	{
		for (int j = 0; j<10; j++)
		{
			cout << map[i][j].getShipPos() << "\t";//...and the starting map
		}
		cout << endl;
	}
	int w = 1;
	do{
		i = 0;
		for (vector<std::unique_ptr<Ship>>::iterator j = v.begin(); j != v.end();){//movement of the ships

			v[i]->Move(v, map, i);
			i++;
			j++;
		}
		i = 0;//ship functions:
		cout << "-------------------------------------------------------------------------\n";
		cout << "SHIP FUNCTIONS:\n";
		cout << "-------------------------------------------------------------------------\n";
		for (vector<std::unique_ptr<Ship>>::iterator j = v.begin(); j != v.end();){//the explorer function must be executed first to avoid pirate ships and weather conditions
			if (v[i]->getType()==4) v[i]->Function(v, map, i);
			i++;
			j++;
		}
		i = 0;
		for (vector<std::unique_ptr<Ship>>::iterator j = v.begin(); j != v.end();){
			if (v[i]->getType() != 4) v[i]->Function(v, map, i);
			if (v[i]->getTreasure() < 0) v[i]->setTreasure(0);//keeping the treasure from falling under 0
			if (v[i]->getCurLife() < 0) v[i]->setCurLife(0);//keeping life from falling under 0
			i++;
			j++;
		}
		cout << "-------------------------------------------------------------------------\n";
		cout << "MAP INTERACTIONS:\n";
		cout << "-------------------------------------------------------------------------\n";
		//every map interaction(weather damage, treasure findings, port functions[repair-damage]-not working yet-)
		i = 0;
		for (vector<std::unique_ptr<Ship>>::iterator j = v.begin(); j != v.end();){
			char* sh = "";
			int x = v[i]->getX();
			int y = v[i]->getY();
			switch (v[i]->getType()){
			case 1:
				sh = "Pirate Ship ";
				break;
			case 2:
				sh = "Merchand Ship ";
				break;
			case 3:
				sh = "Repair Ship ";
				break;
			case 4:
				sh = "Explorer Ship ";
				break;
			}
			if (map[x][y].getWeatherCond() > 8){//damage from weather
				v[i]->setCurLife(v[i]->getCurLife() - 5);
				cout << sh << v[i]->getSnum() << " took 5 damage due to extreme weather conditions.\n";
			}
			if (map[x][y].getTreasurePos() == true){//gaining treasure
				v[i]->setTreasure(v[i]->getTreasure() + 50);
				map[x][y].setTreasurePos(false);
				cout << sh << v[i]->getSnum() << " found a treasure and gained 50 treasure.\n";
			}
			//THIS PART IS NOT WORKING FOR A REASON WE WERE NOT ABLE TO LOCATE BEFORE THE DEADLINE.
			/**int fl = 0;
			for (int lx = -1; lx < 2; lx++){
				for (int ly = -1; ly < 2; ly++){
					if ((x + lx <= 9) && (y + lx >= 0) && (v[i]->getY() + ly <= 9) && (v[i]->getY() + lx >= 0)){     //port damage or repair calculation
						if ((map[x + lx][y + ly].getPortPos() == true) && (fl == 0)){
							if (v[i]->getType() == 1){
								v[i]->setCurLife(v[i]->getCurLife() - 10);
								cout << sh << v[i]->getSnum() << " took 10 damage from a nearby port.\n";
							}
							else{
								if (v[i]->getCurLife() + 4 < v[i]->getMaxLife()){
									v[i]->setCurLife(v[i]->getCurLife() + 5);
									cout << sh << v[i]->getSnum() << " got repaired from a nearby port.\n";
								}
							}
						}
						fl = 1;
					}
				}
			}*/
			if (v[i]->getCurLife() > v[i]->getMaxLife()) v[i]->setCurLife(v[i]->getMaxLife());//keeping the life from going above limits(due to repair from ship or port)
			i++;
			j++;
		}
		cout << "-------------------------------------------------------------------------\n";
		cout << "SHIP STATS:\n";
		cout << "-------------------------------------------------------------------------\n";
		sort(v.begin(), v.end(), srt2);
		i = 0;
		for (vector<std::unique_ptr<Ship>>::iterator j = v.begin(); j != v.end();){
			switch (v[i]->getType()){
			case 1:
				cout << "Pirate Ship ";
				break;
			case 2:
				cout << "Merchand Ship ";
				break;
			case 3:
				cout << "Repair Ship ";
				break;
			case 4:
				cout << "Explorer Ship ";
				break;
			}
			cout << v[i]->getSnum() << " ";
			cout << " new coordinates: " << v[i]->getX() + 1 << ":" << v[i]->getY() + 1 << endl;
			cout << "Ship life:\t" << v[i]->getCurLife() << "/" << v[i]->getMaxLife() << endl;
			cout << "Ship treasure:\t" << v[i]->getTreasure() << endl;
			j++;
			i++;
		}
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				cout << map[i][j].getShipPos() << "\t";
			}
			cout << endl;
		}
		cout << "To continue the simulation press 1\nTo enter the menu press 2\nTo stop the simulation press 0\n";//The menu
		do{
			cin >> w;
			if (w != 0 && w != 1 && w != 2) cout << "You entered a wrong number, please try again:\t";
		} while (w != 0 && w != 1 && w != 2);
		if (w == 2){
			int diepafi;
			do{
				cout << "MENU\n";
				cout << "-----------------------------------------------------------------------\n";
				cout << "To see info about:\n a) a ship, press 1\n b) a location on the map, press 2\n c) a ship type, press 3\n d) simulation statistics, press 4\n";
				cout << "-----------------------------------------------------------------------\n";
				cout << "To add/remove:\n a) a ship, press 5/6\n b) a port, press 7/8\n c) a treasure, press 9/10\n\nTo exit the menu and continue the simulation press 11\nTo terminate the simulation press 0\n";
				cout << "-----------------------------------------------------------------------\n";
				do{
					cin >> diepafi;
					if ((diepafi < 0) || (diepafi>11)) cout << "You entered a wrong number, please try again:\t";
				} while ((diepafi < 0) || (diepafi>11));
				switch (diepafi){
					int l;
					int y, x;
				case 1:
				{
					cout << "Give the coordinates of the ship you want to see info about\n";
					do{
						cin >> x >> y;
						if (x < 1 || y < 1 || x>10 || y>10) cout << "You entered wrong coordinates, please try again:\t";
					} while (x < 1 || y < 1 || x>10 || y>10);
					cout << "-----------------------------------------------------------------------\n";
					i = 0;
					l = 0;
					for (vector<std::unique_ptr<Ship>>::iterator j = v.begin(); j != v.end();){
						if (v[i]->getX() == x - 1 && v[i]->getY() == y - 1){
							l = 1;
							if (v[i]->getType() == 1) cout << "Pirate Ship ";
							else if (v[i]->getType() == 2) cout << "Merchand Ship ";
							else if (v[i]->getType() == 3) cout << "Repair Ship ";
							else cout << "Explorer Ship ";
							cout << "[" << x << ":" << y << "]" << endl << "Treasure: " << v[i]->getTreasure() << endl << "Life: " << v[i]->getCurLife() << "/" << v[i]->getMaxLife() << endl;
							break;
						}
						i++;
						j++;
					}
					if (l == 0) cout << "There is no ship at the location you chose.\n";
					cout << "-----------------------------------------------------------------------\n";
				}
					break;
				case 2:
				{
					cout << "Give the coordinates of the map location you want to see info about\n";
					do{
						cin >> x >> y;
						if (x < 1 || y < 1 || x>10 || y>10) cout << "You entered invalid coordinates, please try again:\t";
					} while (x < 1 || y < 1 || x>10 || y>10);
					cout << "-----------------------------------------------------------------------\n";
					if (map[x - 1][y - 1].getPortPos() == true) cout << "This location is a port so there cannot be any ship or treasure here.\n";
					else{
						cout << "This location is not a port.\n";
						if (map[x - 1][y - 1].getShipPos() == true) {
							i = 0;
							for (vector<std::unique_ptr<Ship>>::iterator j = v.begin(); j != v.end();){
								if ((v[i]->getX() == x - 1) && (v[i]->getY() == y - 1)){
									char* sh = " ";
									switch (v[i]->getType()){
									case 1:
										sh = "Pirate Ship ";
										break;
									case 2:
										sh = "Merchand Ship ";
										break;
									case 3:
										sh = "Repair Ship ";
										break;
									case 4:
										sh = "Explorer Ship ";
										break;
									}
									cout << "There is a ship at this location( " << sh << v[i]->getSnum() << " ).\n";
									break;
								}
								i++;
								j++;
							}
						}
						else cout << "There is no ship at this location.\n";
						if (map[x - 1][y - 1].getTreasurePos() == true) cout << "There is a treasure at this location.\n";
						else cout << "There is no treasure at this location.\n";
					}

					cout << "Volume of weather: " << map[x - 1][y - 1].getWeatherCond() << endl;
					cout << "-----------------------------------------------------------------------\n";
				}
					break;
				case 3:
				{
					int type;
					cout << "For Pirate Ship press 1\nFor Merchant Ship press 2\nFor Repair Ship press 3\nFor Explorer Ship press 4\n";
					do{
						cin >> type;
						if (type != 1 && type != 2 && type != 3 && type != 4) cout << "You entered a wrong number, please try again:\t";
					} while (type != 1 && type != 2 && type != 3 && type != 4);
					cout << "-----------------------------------------------------------------------\n";
					char* sh = " ";
					switch (type){
					case 1:
						cout << "Pirate Ships start the simulation with 100 life and 50 treasure.\nSpeed: 1 block per turn.\nThey cause 10 damage and steal 20 treasure from other ships near them\n";
						cout << "There are these Pirate Ships currently in the simulation:\n";
						sh = "Pirate Ship ";
						break;
					case 2:
						cout << "Merchand Ships start the simulation with 100 life and 100 treasure.\nSpeed : 2 blocks per turn.\nThey search for ports near them and gain 10 treasure from them.\n";
						cout << "There are these Merchand Ships currently in the simulation:\n";
						sh = "Merchand Ship ";
						break;
					case 3:
						cout << "Repair Ships start the simulation with 100 life and 20 treasure.\nSpeed : 2 blocks per turn.\nThey repair ships near them by 5 life and get 10 treasure in exchange.\n";
						cout << "There are these Repair Ships currently in the simulation:\n";
						sh = "Repair Ship ";
						break;
					case 4:
						cout << "Explorer Ships start the simulation with 50 life and 20 treasure.\nSpeed : 2 blocks per turn. (+1 for their function)\nThey search for Pirate ships and extreme weather conditions in order to avoid them\n";
						cout << "There are these Explorer Ships currently in the simulation:\n";
						sh = "Explorer Ship ";
						break;
					}
					i = 0;
					for (vector<std::unique_ptr<Ship>>::iterator j = v.begin(); j != v.end();){
						if (v[i]->getType() == type){
							cout << sh << v[i]->getSnum() << " with coordinates\t" << v[i]->getX() + 1 << ":" << v[i]->getY() + 1 << endl;
						}
						i++;
						j++;
					}
					cout << "For more information about a specific ship use the menu option 1.\n";
					cout << "-----------------------------------------------------------------------\n";
				}
					break;
				case 4:
				{
					cout << "------------------------------------------------------------------------\n";
					cout << "Table of ships currently in the simulation sorted by amount of treasure:\n";
					cout << "------------------------------------------------------------------------\n";
					sort(v.begin(), v.end(), srt);//sorting the vector by treasure
					i = 0;
					for (vector<std::unique_ptr<Ship>>::iterator j = v.begin(); j != v.end();){
						cout << i + 1 << ")  ";
						switch (v[i]->getType()){
						case 1:
							cout << "Pirate Ship ";
							break;
						case 2:
							cout << "Merchand Ship ";
							break;
						case 3:
							cout << "Repair Ship ";
							break;
						case 4:
							cout << "Explorer Ship ";
							break;
						}
						cout << v[i]->getSnum() << " with amount of treasure: " << v[i]->getTreasure() << " and current life: " << v[i]->getCurLife() << "/" << v[i]->getMaxLife() << endl;
						i++;
						j++;
					}
					cout << "-----------------------------------------------------------------------\n";
				}
					break;
				case 5:
				{
					int ship;
					cout << "For Pirate Ship press 1\nFor Merchant Ship press 2\nFor Repair Ship press 3\nFor Explorer Ship press 4\n";
					do{
						cin >> ship;
						if (ship != 1 && ship != 2 && ship != 3 && ship != 4) cout << "You entered a wrong number, please try again:\t";
					} while (ship != 1 && ship != 2 && ship != 3 && ship != 4);
					cout << "-----------------------------------------------------------------------\n";
					switch (ship){//the ships start from 0:0 as set at the constructor of the ship class and move randomly after the next round
					case 1:
						v.emplace_back(new PirateShip);
						cout << "New Pirate Ship added\n";
						break;
					case 2:
						v.emplace_back(new MerchandShip);
						cout << "New Merchand Ship added\n";
						break;
					case 3:
						v.emplace_back(new RepairShip);
						cout << "New Repair Ship added\n";
						break;
					case 4:
						v.emplace_back(new ExplorerShip);
						cout << "New Explorer Ship added\n";
						break;
					}
					cout << "-----------------------------------------------------------------------\n";
				}
					break;
				case 6:
				{
					cout << "Enter the coordinates of the ship you want to remove\n";
					do{
						cin >> x >> y;
						if (x < 1 || y < 1 || x>10 || y>10) cout << "You entered invalid coordinates, please try again:\t";
					} while (x < 1 || y < 1 || x>10 || y>10);
					cout << "-----------------------------------------------------------------------\n";
					i = 0;
					l = 0;
					for (vector<std::unique_ptr<Ship>>::iterator j = v.begin(); j != v.end();){
						if (v[i]->getX() == x - 1 && v[i]->getY() == y - 1){
							map[v[i]->getX()][v[i]->getY()].setShipPos(false);
							map[v[i]->getX()][v[i]->getY()].setTreasurePos(true);
							v.erase(v.begin() + i);
							l = 1;
							cout << "Ship removed.\n";
							break;
						}
						i++;
						j++;
					}
					if (l == 0) cout << "There is no ship on the coordinates you entered!\n";
					cout << "-----------------------------------------------------------------------\n";
				}
					break;
				case 7:
				{
					cout << "Give coordinates of the new port\n";
					do{
						cin >> x >> y;
						if (x < 1 || y < 1 || x>10 || y>10) cout << "You entered invalid coordinates, please try again:\t";
					} while (x < 1 || y < 1 || x>10 || y>10);
					cout << "-----------------------------------------------------------------------\n";
					if ((map[x - 1][y - 1].getPortPos() == false) && (map[x - 1][y - 1].getTreasurePos() == false) && (map[x - 1][y - 1].getShipPos() == false)){
						map[x - 1][y - 1].setPortPos(true);
						cout << "New port added on coordinates: " << x << ":" << y << endl;
					}
					else{
						cout << "The location you selected is already a port or contains a treasure/ship!\n";
					}
					cout << "-----------------------------------------------------------------------\n";
				}
					break;
				case 8:
				{
					cout << "Give coordinates of the port you want to remove\n";
					do{
						cin >> x >> y;
						if (x < 1 || y < 1 || x>10 || y>10) cout << "You entered invalid coordinates, please try again:\t";
					} while (x < 1 || y < 1 || x>10 || y>10);
					cout << "-----------------------------------------------------------------------\n";
					if (map[x - 1][y - 1].getPortPos() == true){
						map[x - 1][y - 1].setPortPos(false);
						cout << "Port removed.";
					}
					else{
						cout << "The location you selected is not a port!\n";
					}
					cout << "-----------------------------------------------------------------------\n";
				}
					break;
				case 9:
				{
					cout << "Give coordinates of the treasure you want to create\n";
					do{
						cin >> x >> y;
						if (x < 1 || y < 1 || x>10 || y>10) cout << "You entered invalid coordinates, please try again:\t";
					} while (x < 1 || y < 1 || x>10 || y>10);
					cout << "-----------------------------------------------------------------------\n";
					if ((map[y - 1][x - 1].getPortPos() == false) && (map[y - 1][x - 1].getTreasurePos() == false)){
						map[y - 1][x - 1].setTreasurePos(true);
						cout << "New treasure added on coordinates: " << y << ":" << x << endl;
					}
					else{
						cout << "The location you selected already contains a treasure or is a port!\n";
					}
					cout << "-----------------------------------------------------------------------\n";
				}
					break;
				case 10:
				{
					cout << "Give coordinates of the treasure you want to remove\n";
					do{
						cin >> x >> y;
						if (x < 1 || y < 1 || x>10 || y>10) cout << "You entered invalid coordinates, please try again:\t";
					} while (x < 1 || y < 1 || x>10 || y>10);
					cout << "-----------------------------------------------------------------------\n";
					if (map[x - 1][y - 1].getTreasurePos() == true){
						map[x - 1][y - 1].setTreasurePos(false);
						cout << "Treasure removed.";
					}
					else{
						cout << "The location you selected does not contain a treasure!\n";
					}
					cout << "-----------------------------------------------------------------------\n";
				}
					break;
				}
				if ((diepafi != 11) && (diepafi != 0)){
					cout << "\nTo continue the simulation press 1\nTo return to the menu press 2\nTo stop the simulation press 0\n";
					cin >> diepafi;
					if (diepafi == 1) w = 1;
					if (diepafi == 0) w = 0;
				}
			} while (diepafi == 2);
			if (diepafi == 11) w = 1;
			if (diepafi == 0) w = 0;
		}
		i = 0;
		for (vector<std::unique_ptr<Ship>>::iterator j = v.begin(); j != v.end();){
			if (v[i]->getCurLife() <= 0) {
				cout << "\nIMPORTANT:\n";
				cout << "-------------------------------------------------------------------------\n";
				switch (v[i]->getType()){
				case 1:
					cout << "Pirate Ship ";
					break;
				case 2:
					cout << "Merchand Ship ";
					break;
				case 3:
					cout << "Repair Ship ";
					break;
				case 4:
					cout << "Explorer Ship ";
					break;
				}
				cout << v[i]->getSnum() << " is removed from the simulation because its life reached 0!\n";
				cout << "-------------------------------------------------------------------------\n";
				map[v[i]->getX()][v[i]->getY()].setShipPos(false);
				map[v[i]->getX()][v[i]->getY()].setTreasurePos(true);
				v.erase(v.begin() + i);//removing a ship that has no life remaining
				break;
			}
			i++;
			j++;
		}
		int rew;
		for (int ky = 0; ky < 10; ky++){//renewing the weather conditions
			for (int kx = 0; kx < 10; kx++){
				rew = rand() % 2;
				if (rew == 0) map[ky][kx].setWeatherCond(map[ky][kx].getWeatherCond() - 1);
				else map[ky][kx].setWeatherCond(map[ky][kx].getWeatherCond() + 1);
				if (map[ky][kx].getWeatherCond() > 10) map[ky][kx].setWeatherCond(10);
				if (map[ky][kx].getWeatherCond() < 1) map[ky][kx].setWeatherCond(1);
			}
		}
		t = 0;
		i = 0;
		for (vector<std::unique_ptr<Ship>>::iterator j = v.begin(); j != v.end();){//checking if the treasure goal is reached
			if (v[i]->getTreasure() > t) t = v[i]->getTreasure();
			i++;
			j++;
		}
	} while ((w == 1) && (t < goal) && (v.size() > 1));
	if (t >= goal){
		sort(v.begin(), v.end(), srt);
		char* sh = " ";
		switch (v[0]->getType()){
		case 1:
			sh = "Pirate Ship ";
			break;
		case 2:
			sh = "Merchand Ship ";
			break;
		case 3:
			sh = "Repair Ship ";
			break;
		case 4:
			sh = "Explorer Ship ";
			break;
		}
		cout << "\n\n-------------------------------------------------------------------------\n";
		cout << "SIMULATION COMPLETED!\n";
		cout << "-------------------------------------------------------------------------\n";
		cout << "The " << sh << v[0]->getSnum() << " reached the goal of 700 treasure!\n";
		cout << "-------------------------------------------------------------------------\n";
		cout << "-------------------------------------------------------------------------\n";
	}
	if (v.size() == 1){
		char* sh = " ";
		switch (v[0]->getType()){
		case 1:
			sh = "Pirate Ship ";
			break;
		case 2:
			sh = "Merchand Ship ";
			break;
		case 3:
			sh = "Repair Ship ";
			break;
		case 4:
			sh = "Explorer Ship ";
			break;
		}
		cout << "\n\n-------------------------------------------------------------------------\n";
		cout << "SIMULATION COMPLETED!\n";
		cout << "-------------------------------------------------------------------------\n";
		cout << "The " << sh << v[0]->getSnum() << " is the only ship remaining!\n";
		cout << "-------------------------------------------------------------------------\n";
		cout << "-------------------------------------------------------------------------\n";
	}
	if (w == 0){
		sort(v.begin(), v.end(), srt);
		cout << "\n\n-------------------------------------------------------------------------\n";
		cout << "SIMULATION STOPPED BY THE USER!\n";
		cout << "-------------------------------------------------------------------------\n";
		cout << "The classification of the ships at the time the simulation ended was:\n";
		cout << "-------------------------------------------------------------------------\n";
		cout << "-------------------------------------------------------------------------\n";
		i = 0;
		for (vector<std::unique_ptr<Ship>>::iterator j = v.begin(); j != v.end();){
			cout << i + 1 << ")  ";
			switch (v[i]->getType()){
			case 1:
				cout << "Pirate Ship ";
				break;
			case 2:
				cout << "Merchand Ship ";
				break;
			case 3:
				cout << "Repair Ship ";
				break;
			case 4:
				cout << "Explorer Ship ";
				break;
			}
			cout << v[i]->getSnum() << " with amount of treasure: " << v[i]->getTreasure() << " and current life: " << v[i]->getCurLife() << "/" << v[i]->getMaxLife() << endl;
			i++;
			j++;
		}
	}
	cout << "\nPress Enter to exit.\n";
	cin.get();
	cin.get();
	return 0;
}