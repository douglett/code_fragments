#include <iostream>
#include <sstream>
#include <vector>

using namespace std;


class Mob {
public:
	string name;
	int hp = 0;
	int mp = 0;
	int atk = 0;
	int def = 0;
};


Mob makemob(string type) {
	Mob mob;
	if (type == "player") {
		mob.name = "player";
		mob.hp = 20;
		mob.atk = 2;
		mob.def = 0;
	} else if (type == "goat") {
		mob.name = "goat";
		mob.hp = 5;
		mob.atk = 1;
		mob.def = 0;
	}
	return mob;
}


// int* attack(Mob& mob1, Mob& mob2) {
// 	static int damage[2];
// 	// first attack
// 	damage[0] = mob1.atk - mob2.def;
// 	mob2.hp -= damage[0];
// 	if (mob2.hp <= 0) {
// 		mob2.hp = 0;
// 		damage[1] = -1;
// 		return damage;
// 	}
// 	// second attack
// 	damage[1] = mob2.atk - mob1.def;
// 	mob1.hp -= damage[1];
// 	if (mob1.hp <= 0) {
// 		mob1.hp = 0;
// 	}
// 	return damage;
// }


// void show(Mob& mob) {
// 	cout 
// 		<< "hp:  " << mob.hp << endl
// 		<< "atk: " << mob.atk << endl
// 		<< "def: " << mob.def << endl << endl;
// }


string showsmall(Mob& mob) {
	static stringstream ss;
	ss.str(""), ss.clear();
	ss << mob.hp << " (" << mob.atk << "/" << mob.def << ")";
	return ss.str();
}


int attack(Mob& mob1, Mob& mob2) {
	int damage = mob1.atk - mob2.def;
	mob2.hp -= damage;
	if (mob2.hp <= 0)
		mob2.hp = 0;
	return damage;
}


int attack_txt(Mob& mob1, Mob& mob2) {
	int damage = attack(mob1, mob2);
	printf("%s attacks %s for %d.\n", mob1.name.c_str(), mob2.name.c_str(), damage);
	if (mob2.hp == 0)
		printf("%s dies.\n", mob2.name.c_str());
	return damage;
}


void attack_counter_txt(Mob& mob1, Mob& mob2) {
	// first attack
	attack_txt(mob1, mob2);
	if (mob2.hp == 0)
		return;
	// counter attack
	attack_txt(mob2, mob1);
}


int main() {
	cout << "hello world" << endl;

	Mob player = makemob("player");
	vector<Mob> moblist;
	for (int i = 0; i < 3; i++)
		moblist.push_back( makemob("goat") );

	// string s;
	while (true) {
		// show game state
		cout << endl;
		int alive = 0;
		if (moblist.size() > 0) {
			for (int i = 0; i < moblist.size(); i++)
				if ( moblist[i].hp > 0 ) {
					alive++;
					printf( "%s(%d), ", moblist[i].name.c_str(), i+1 );
				}
		}
		if (alive == 0) {
			cout << "(nothing here)";
		}
		cout << endl;

		// show player stats
		cout << showsmall(player) << ": ";
		
		// get input
		int num = 0;
		cin >> num;
		cin.clear();
		cin.ignore(10000,'\n');
		
		if (num > 0 && num <= moblist.size()) {
			auto mob = moblist.begin() + num-1;
			if (mob->hp > 0) {
				attack_counter_txt( player, moblist[num-1] );
				continue;
			}
		}

		cout << "enemy select fail" << endl;
	}
}