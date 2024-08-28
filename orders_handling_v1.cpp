//orders handling c++ v1--the basics
#include <iostream>
#include <ctime>
#include <chrono>
#include <string>
#include <vector>

//the constants for prices time or cost
constexpr int PEND = 0;
constexpr int DEL = 1;
constexpr int FIN = 2;
constexpr float DISTANCE_FEE = 0.25;
constexpr float SERVICE_FEE =  0.5;
constexpr int EASY = 10;
constexpr int MEDIUM = 15;
constexpr int HARD = 20;
constexpr float GYROS = 1.1;
constexpr int CHEESE_BURGER = 3;
constexpr int WINGS = 2;
constexpr float KEBAB = 1.50;

class Product {
private:
	std::string name;
	double price;
	float contruction_cost;
	double time_to_make;//in minutes
public:
	Product():name("string not found"),price(0),contruction_cost(0),time_to_make(0)
	{

	}
	

	Product(std::string& name, double price, float cost, float time_to_make,bool fries) {
		this->contruction_cost = cost;
		this->name = name;
		this->price = price;
		this->time_to_make = time_to_make;
	}

	Product(const Product& p)
		: name(p.name), price(p.price), contruction_cost(p.contruction_cost),
		time_to_make(p.time_to_make) {}

	std::string getName() {
		return this->name;
	}

	void set_name(std::string name) {
		this->name = name; 
	}

	double getPrice() {
		return price;
	}

	double getCost() {
		return contruction_cost;
	}

	double getTime() {
		return time_to_make;
	}
};

class Order {
private:
	std::string name;
	std::string adress;
	int travel_time;
	std::vector<Product> products;
	int day;
	int month;
	int year;
	int status;
	bool fries;
	double price;
	float cost;
	float preparation_time;
public:
	Order() :day(0), month(0), year(0), name("unknown"), adress("unknown"), travel_time(0),fries(false),price(0),cost(0)
	{
		this->products.reserve(10);
		this->status = PEND;
		this->preparation_time = 0;
	}

	Order(std::string& name, std::string& adress,int status,Product* products, int size, int day, int month, int year,bool fries) {
		this->name = name;
		this->adress = adress;
		this->day = day;
		this->month = month;
		this->year = year;
		this->status = status;
		this->fries = fries;
		this->cost = 0;
		this->preparation_time = 0;
		this->price = 0;

		for (int i = 0; i < size; i++) {
			this->products.push_back(Product(products[i]));
		}

		this->travel_time = 3 + rand() % 45;
	}

	void setStatus(int status) {
		this->status = status;
	}

	std::string get_name() { 
		return this->name; 
	}

	std::string get_adress() {
		return this->adress;
	}

	double get_deliveryTime() {
		return this->travel_time;
	}

	Product get_product(int index) {
		return this->products[index];
	}

	std::string get_date() {
		//std::cout << "\n" << this->day << "\\" << this->month << "\\" << this->year << "\n";
		std::string date = std::to_string(this->day);
		date += "\\";
		date += std::to_string(this->month);
		date += "\\";
		date += std::to_string(this->year);
		return date;
	}

	std::vector<Product> getVector(){
		return this->products;
	}

	bool fries_included() {
		return this->fries;
	}
	void check() {
		std::cout << this->name << " " << this->products[0].getName();
	}

	float get_cost() {
		return this->cost;
	}

	float get_preparationTime() {
		return this->preparation_time;
	}

	double get_price() {
		return this->price;
	}

	int get_travelTime() {
		return this->travel_time;
	}

	int getstatus() {
		return this->status;
	}
};


int p_size = 10;
int r_size = 10;
int f_size = 10;
int p = -1;//tracks the number of active pending orders
int r = -1;//tracks the number of active ready orders
int f = -1;// tracks the number of active finished orders


void options(char& choice);
void  print(Order* &array, int quantity,int status);
void add_order(Order *&order);
void menu();
void increaseSize(Order *&array, int& current_size);
void toready(Order* &og, Order* &new_);
void tofinished(Order* og, Order* new_);
void printAll(Order*& pend, Order*& ready, Order*& fin);
void defaultOrder(Order*&pending);

int main() {
	std::cout << "order handling app v1\n";
	std::cout << "=====================\n";

	//assets
	Order* pending = new Order[p_size];//starts with space for 10 pending orders
	Order* ready = new Order[r_size];//starts with space for 10 ready orders
	Order* finished = new Order[f_size];//starts with space for 10 pending orders

	//the loop
	bool run = true;
	char choice;
	while (run) {
		options(choice);//selects the task you want to do

		switch (choice) {
		case 'x':
			std::cout << "\nprogram is closing\n";
			run = false;
			break;
		case '+':
			add_order(pending);
			break;
		case '-':
			//cancel order
			print(pending, p, PEND);
			break;
		case '=':
			print(pending,p, PEND);
			break;
		case '&':
			print(ready, r, DEL);
			break;
		case '%':
			print(finished,f,FIN);
			break;
		case '!':
			printAll(pending, ready, finished);
			break;
		case 'p':
			toready(pending,ready);
			break;
		case 'd':
			tofinished(ready,finished);
			break;
		case '$':
			// money made today
			break;
		case '0':
			//adds a default order to pening for testing
			defaultOrder(pending);
			//std::cout << p;
			break;
		default:
			std::cout << "\nsorry thats not an option please try another one\n";
			break;
		}
	}

	return 0;
}

void increaseSize(Order *& array, int& current_size){
	int ogsize = current_size;
	current_size *= 2;
	Order* arr = new(std::nothrow) Order[current_size];

	if (!arr) {
		std::cerr << "Memory allocation failed during resize.\n";
		return;
	}

	for (int i = 0; i < ogsize; i++)  arr[i]=array[i];
	
	delete[] array;
	array = arr;
}

void defaultOrder(Order*& pending) {
	std::string name="theo adams";
	std::string adress = "ousani 6";
	std::string meal = "pork gyros";
	Product* food = new Product[3];
	for (int i = 0; i < 3; i++) {
		food[i] = Product(meal, 3.2, GYROS, EASY, true);
	}

	p++;
	if ((p + 1) == p_size) {
		increaseSize(pending,p_size);
	}
	pending[p] = Order(name, adress, PEND, food, 3, 28, 8, 2024, true);
	
	delete[] food; 
	std::cout << "\nsuccesfully added default order\n";
}

void toready(Order*&og,Order*&new_) {
	print(og, p, PEND);
	std::cout << "\nselect one of the above orders to be prepared for delivery by typing its order number:";
	int num;
	std::cin >> num;
	//check if number exceeds the pending size
	num--;
	if (num >= 0 && num <= p) {
		r++;
		if ((r + 1) == r_size) increaseSize(new_, r_size);
		new_[r] = og[num];
		new_[r].setStatus(DEL);
		og[num] = og[p];
		p--;//reduce the number of elemnts in the original array
		std::cout << "\norder succesfully moved from pending to ready \n";
	}
	else {
		std::cout << "\nout of bounds\n";
	}
}

void tofinished(Order*og,Order*new_) {
	print(og, r, DEL);
	std::cout << "\nselect one of the above orders to be prepared to deliver by typing its order number:";
	int num;
	std::cin >> num;
	//check if number exceeds the ready orders size
	num--;
	if (num>=0 && num <= r) {
		f++;
		if((f+1)==f_size)
		new_[f] = og[num];
		new_[f].setStatus(FIN);
		og[num] = og[r];
		r--;//reduce the number of elemnts in the original array
		std::cout << "\norder succesfully moved from ready to finished \n";
	}
	else {
		std::cout << "\nout of bounds\n";
	}
}

void options(char& choice) {
	std::cout << "\nplease select one of the following options you would like to do\n";
	std::cout << "----------------------------------------------------------------\n";
	std::cout << "x -> closes the program\n";
	std::cout << "+ -> makes a new order\n";
	std::cout << "- -> cancels an order\n";
	std::cout << "= -> prints all the pending orders\n";
	std::cout << "& -> prints all the orders that are ready for delivery\n";
	std::cout << "% -> prints all the orders that have been delievered\n";
	std::cout << "! -> prints all the orders with their statuses\n";
	std::cout << "p -> prepares an pending order for delivery\n";
	std::cout << "d -> delivers an order and receive payment\n";
	std::cout << "$ ->to get statistics of todays profits\n";
	std::cout << "\n";
	std::cout << "@&%&%@&@%&%@&@%@&%@&@%&@%&@%@&%@%@&%@&@%&%@&%@&%%&%@&%@&%@*@%@*@\n";
	std::cout << "\nselect your option:";
	std::cin >> choice;
}

void printAll(Order*&pend,Order*&ready,Order*&fin) {
	print(pend, p, PEND);
	print(ready, r, DEL);
	print(fin, f, FIN);
}

void  print(Order* &data, int quantity,int status)
{
	if (quantity >=0){
		switch (status) {
		case PEND:
			std::cout << "\n";
			std::cout << "++++++++++++++++++++++++++++++++++++++++++++++\n";
			for (int i = 0; i <= quantity; i++) {
				std::cout << "order no:" << i + 1 << " name:" << data[i].get_name() << " adress:" << data[i].get_adress() << " date:" << data[i].get_date() << "\n";
				std::cout << "ordered:\n";
				for (int j = 0; j < data[i].getVector().size(); j++) {
					std::cout << "* " << data[i].get_product(j).getName() << " " << data[i].get_product(j).getPrice() << " $\n";
				}
				(data[i].fries_included()) ? std::cout << "fries included\n\n" : std::cout << "fries not included\n\n";
			}
			std::cout << "+++++++++++++++++++++++++++++++++++++++++++++\n";
			std::cout << "----above you have all the pending orders----\n";
			break;
		case DEL:
			std::cout << "\n";
			std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
			for (int i = 0; i <= quantity; i++) {
				std::cout << "order no:" << i + 1 << " name:" << data[i].get_name() << " adress:" << data[i].get_adress() << " date:" << data[i].get_date() << "\n";
				std::cout << "ordered:\n";
				for (int j = 0; j < data[i].getVector().size(); j++) {
					std::cout << "* " << data[i].get_product(j).getName() << " " << data[i].get_product(j).getPrice() << " $\n";
				}
				(data[i].fries_included()) ? std::cout << "fries included\n" : std::cout << "fries not included\n";
				std::cout <<"price:"<<data[i].get_price()<<" $ " << "time to prepare the order:" << data[i].get_preparationTime() << " minutes\n";
			}
			std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
			std::cout << "----above you have all the orders that are ready for delivery----\n";
			break;
		case FIN:
			std::cout << "\n";
			std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++\n";
			for (int i = 0; i <= quantity; i++) {
				std::cout << "order no:" << i + 1 << " name:" << data[i].get_name() << " adress:" << data[i].get_adress() << " date:" << data[i].get_date() << "\n";
				std::cout << "ordered:\n";
				for (int j = 0; j < data[i].getVector().size(); j++) {
					std::cout << "* " << data[i].get_product(j).getName() << " " << data[i].get_product(j).getPrice() << " $\n";
				}
				(data[i].fries_included()) ? std::cout << "fries included\n" : std::cout << "fries not included\n";
				std::cout << "price:" << data[i].get_price() << " $ " << "time to prepare the order:" << data[i].get_preparationTime() << " minutes\n";
				std::cout << "delivery time:" << data[i].get_deliveryTime() << "construction cost:" << data[i].get_cost() << " $\n";
			}
			std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++\n";
			std::cout << "----above you have all the delivered orders----\n";
			break;
		}
		
	}
	else std::cout << "\n=========no available orders=========\n";
}

void add_order(Order *&order) {
	std::cout << "\nenter your name:";
	std::string name;
	std::getline(std::cin>>std::ws, name);
	std::cout << "enter your adress:";
	std::string adress;
	std::getline(std::cin>>std::ws, adress);
	bool print_menu = true;

	std::string product_name;
	int quantity;
	bool fries;
	int dificulty;
	double price;
	float cost;

	while (print_menu) {
		menu();
		std::cout << "\nenter your option by picking an number:";
		char choice;
		std::cin >> choice;
		print_menu = false;
		switch (choice) {
		case '1':
			std::cout << "\nhow many servings would you like:";
			std::cin >> quantity;
			product_name = "pork gyros";
			dificulty = EASY;
			price = 3.20;
			cost = GYROS;
			break;
		case '2':
			std::cout << "\nhow many servings would you like:";
			std::cin >> quantity;
			product_name = "chicken gyros";
			dificulty = EASY;
			price = 3.2;
			cost = GYROS;
			break;
		case '3':
			std::cout << "\nhow many servings would you like:";
			std::cin >> quantity;
			product_name = "cheeseburger";
			dificulty = HARD;
			price = 6;
			cost = CHEESE_BURGER;
			break;
		case '4':
			std::cout << "\nhow many servings would you like:";
			std::cin >> quantity;
			product_name = "chicken wings";
			dificulty = MEDIUM;
			price = 5.10;
			cost = WINGS;
			break;
		case '5':
			std::cout << "\nhow many servings would you like:";
			std::cin >> quantity;
			product_name = "kebab";
			dificulty = HARD;
			price = 6;
			cost = KEBAB;
			break;
		default:
			std::cout << "im sorry pick again thats not an option\n";
			print_menu = true;
			break;
		}
	}

	std::string answer;
	do {
		std::cout << "\nwant fries with that(y\\n):";
		std::cin >> answer;
		fflush(stdin);
	} while (answer != "n" && answer != "y");

	(answer == "y") ? fries = true : fries = false;

	Product* food = new Product[quantity];

	for (int i = 0; i < quantity; i++) {
		food[i] = Product(product_name,price,cost,dificulty,fries);
	}

	auto now = std::chrono::system_clock::now();

	std::time_t now_c = std::chrono::system_clock::to_time_t(now);

	std::tm now_tm;

	if (localtime_s(&now_tm, &now_c) != 0) {
		std::cerr << "Error converting time." << std::endl;
		return ;
	}

	p++;
	//resize when out of memmory
	if ((p + 1) == p_size) increaseSize(order, p_size);
	
	order[p]=Order(name,adress,PEND,food,quantity,now_tm.tm_mday,now_tm.tm_mon+1,now_tm.tm_year+1900,fries);
	
	delete[] food;
}

void menu() {
	std::cout << "\n=========================================================================\n";
	std::cout << "1.pork gyros -> 3.20$\n";
	std::cout << "2.chicken gyros ->3.20$\n";
	std::cout << "3.cheeseburger (200 gr beef)->6$\n";
	std::cout << "4.chicken wings (4 wings) -> 5.10$\n";
	std::cout << "5.kebab (200 gr)->6$\n";
	std::cout << "(you can add a serving of fries on the side to the above meals for 2.50$)\n";
	std::cout << "if you ordered 3 servings or more you get 5% off the final price\n";
	std::cout << "\n=========================================================================\n";
}