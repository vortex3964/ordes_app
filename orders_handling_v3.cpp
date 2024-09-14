//orders handling c++ v3--adding the worker class that simulates a restaurant
#include <iostream>
#include <ctime>
#include <chrono>
#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include <mutex>

//the constants for prices time or cost
constexpr int PEND = 0;
constexpr int DEL = 1;
constexpr int FIN = 2;
constexpr double DISTANCE_FEE = 0.25;
constexpr double SERVICE_FEE = 0.5;
constexpr int EASY = 10;
constexpr int MEDIUM = 15;
constexpr int HARD = 20;
constexpr float GYROS = 1.1;
constexpr int CHEESE_BURGER = 3;
constexpr int WINGS = 2;
constexpr float KEBAB = 1.50;
constexpr int TOO_SMALL = 10;

int p_size = 10;
int r_size = 10;
int f_size = 10;
int p = -1;//tracks the number of active pending orders
int r = -1;//tracks the number of active ready orders
int f = -1;// tracks the number of active finished orders
double losses = 0;//amount of money paid to make the food
double gains = 0;//amount we made by delivering it
const int shefs_size = 1;//how many shefs we have hired
const int delivery_men_size = 1;//how many delivery men we have hired

class Product {
private:
	std::string name;
	double price;
	float contruction_cost;
	double time_to_make;//in minutes
public:
	Product() :name("string not found"), price(0), contruction_cost(0), time_to_make(0)
	{

	}

	Product(std::string& name, double price, float cost, float time_to_make) {
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
	bool duplicate;
public:
	Order() :day(0), month(0), year(0), name("unknown"), adress("unknown"), travel_time(0), fries(false), price(0), cost(0), duplicate(false)
	{
		this->products.reserve(10);
		this->status = PEND;
		this->preparation_time = 0;
	}

	Order(std::string& name, std::string& adress, int status, Product* products, int size_of_p, int day, int month, int year, bool fries) {
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
		duplicate = false;

		for (int i = 0; i < size_of_p; i++) {
			this->products.push_back(Product(products[i]));
		}

		this->travel_time = 3 + rand() % 45;
	}

	Order(std::string& name, std::string& adress, int status, std::vector<Product>& products, int day, int month, int year, bool fries) {
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
		duplicate = false;

		this->products = products;
		this->travel_time = 3 + rand() % 45;
	}

	void setStatus(int status) {
		this->status = status;
	}

	std::string get_name() {
		return name;
	}

	std::string get_adress() {
		return adress;
	}

	void setDelivery_time(int delivery_time) {
		this->travel_time = delivery_time;
	}

	double get_deliveryTime() {
		return travel_time;
	}

	Product get_product(int index) {
		return products[index];
	}

	std::string get_date() {
		//std::cout << "\n" << this->day << "\\" << this->month << "\\" << this->year << "\n";
		std::string date = std::to_string(day);
		date += "\\";
		date += std::to_string(month);
		date += "\\";
		date += std::to_string(year);
		return date;
	}

	std::vector<Product> getVector() {
		return products;
	}

	bool fries_included() {
		return fries;
	}

	float get_cost() {
		return cost;
	}

	float get_preparationTime() {
		return preparation_time;
	}

	double get_price() {
		return price;
	}

	int get_travelTime() {
		return travel_time;
	}

	int getstatus() {
		return status;
	}

	bool isduplicate() {
		return duplicate;
	}
	void setdupped(bool d) {
		duplicate = d;
	}

	void setCost(float cost) {
		this->cost = cost;
	}

	void setPrice(double price) {
		this->price = price;
	}

	void setprep_time(float preparation_time) {
		this->preparation_time = preparation_time;
	}
};

std::condition_variable cv;
std::mutex onlyOne;
std::atomic<bool> stop{ false };

class Worker {
private:
	std::string name;
	static float salary;
	int stamina = 10;
	int work_load = 0;
public:
	Worker() {
		name = "unknown";
	}

	virtual void work(Order& order, Order*& destination, Order*& source, int& size, int& size_d) = 0;

	void LookforOrder(Order*& destination, Order*& source, int& size, int& size_d) {
		// Loop until stopFlag is set
		while (!stop.load()) {
			std::unique_lock<std::mutex> lock(onlyOne);
			cv.wait(lock, [&size] { return (size > -1) || stop.load(); });

			if (stop.load()) {  // If stopFlag is set, exit the loop
				break;
			}

			// Proceed if an order is available
			if (size > -1) {
				std::this_thread::sleep_for(std::chrono::seconds(2));//sleep for 2 seconds
				Order& order = source[size];
				size--; // Take out the last order from our array
				cv.notify_all();
				lock.unlock();
				this->work(order, destination, source, size, size_d);
			}
		}
	}

	Worker(std::string& name) {
		this->name = name;
	}

	static float getDSalary() {
		return salary;
	}

	void increaseWorkLoad() {
		work_load++;
	}

	void resetWorkLoad() {
		work_load = 0;
	}

	int getWorkLoad() {
		return work_load;
	}

	void getTired() {
		stamina--;
	}

	std::string getName() {
		return name;
	}

	int  getStamina() {
		return stamina;
	}
};

class Shef :public Worker {
public:
	Shef() {}

	Shef(std::string& name) :Worker(name) {}

	//gets orders from pending to ready
	void work(Order& order, Order*& destination, Order*& source, int& size, int& size_d) {

		this->increaseWorkLoad();
		if (this->getWorkLoad() == 3) {
			this->resetWorkLoad();
			if (this->getStamina() >= 1) this->getTired();
		}
		std::this_thread::sleep_for(std::chrono::seconds((int)order.get_preparationTime() / (this->getStamina() * 2)));

		this->Movetoready(order, destination, r_size, size_d);
		this->LookforOrder(destination, source, size, size_d);
		
	}

	void Movetoready(Order& order, Order*& destination, int& quant, int& size);
};

class Deliveryman :public Worker {
public:
	Deliveryman() {}

	Deliveryman(std::string& name) :Worker(name) {}

	//gets orders from ready to finished
	void work(Order& order, Order*& destination, Order*& source, int& size, int& size_d) {
		
		this->increaseWorkLoad();
		if (this->getWorkLoad() == 3) {
			this->resetWorkLoad();
			if (this->getStamina() >= 0) this->getTired();
		}

		std::this_thread::sleep_for(std::chrono::seconds((int)order.get_deliveryTime() / (this->getStamina() * 2)));


		this->Movetofinished(order, destination, f_size, size_d);
		this->LookforOrder(destination, source, size, size_d);
	}

	void Movetofinished(Order& order, Order*& destination, int& quant, int& size);
};

void add_order(Order*& order);
void add_order(Order*& order, std::vector<Order>& new_, int& track, int& sizeof_array);

void menu();
void options(char& choice);
void refresh();
void increaseSize(Order*& array, int& current_size);

void toready(Order*& og, Order*& new_);
void tofinished(Order* og, Order* new_);

void  print(Order*& array, int quantity, int status);
void printAll(Order*& pend, Order*& ready, Order*& fin);
void defaultOrder(Order*& pending);
void exportdata(Order* pending, Order* ready, Order* finished);
void importPend(Order*& pending);//
void importReady(Order*& ready);
void importFin(Order*& finished);

void calculate_fprice(Order& order);
void calculate_cost(Order& order);
void calculate_prep_time(Order& order);
void save_profit();

//sorting functs
void sort(Order*& array, int size, int start, int end);
void insertionSort(Order*& array, int size);
void quickSort(Order*& array, int start, int end);

double calc_travel_fee(Order& order);
void calcTemp_price(Order& new_);

bool run = true;//the status of the program
float Worker::salary = 20.2;

int main() {
	//assets
	Order* pending = new Order[p_size];//starts with space for 10 pending orders
	Order* ready = new Order[r_size];//starts with space for 10 ready orders
	Order* finished = new Order[f_size];//starts with space for 10 pending orders
	Shef shef = Shef();
	Deliveryman deliveryman = Deliveryman();

	//test
	std::thread st(&Shef::LookforOrder, &shef, std::ref(ready), std::ref(pending), std::ref(p), std::ref(r));
	std::thread dt(&Deliveryman::LookforOrder, &deliveryman, std::ref(finished), std::ref(ready), std::ref(r), std::ref(f));
	//the loop
	char choice;
	while (run) {
		options(choice);//selects the task you want to do
		
		switch (choice) {
		case 'x':
			std::cout << "\nprogram is closing\n";
			run = false;
			stop.store(true);
			cv.notify_all();
			//stopFlag = true;
			if (st.joinable()) st.join();
			if (dt.joinable()) dt.join();
			save_profit();
			break;
		case '+':
			add_order(pending);
			cv.notify_all();
			break;
		case 'p':
			print(pending, p, PEND);
			break;
		case 'd':
			print(ready, r, DEL);
			break;
		case 'f':
			print(finished, f, FIN);
			break;
		case '=':
			printAll(pending, ready, finished);
			break;
		case '\\':
			toready(pending, ready);
			break;
		case '!':
			tofinished(ready, finished);
			break;
		case 'e':
			sort(pending, p, 0, p - 1);
			sort(ready, r, 0, r - 1);
			sort(finished, f, 0, f - 1);
			exportdata(pending, ready, finished);
			std::cout << "finished exporting data\n";
			break;
		case 'i':
			importPend(pending);
			importReady(ready);
			importFin(finished);
			std::cout << "finished imporing all the data\n";
			break;
		case 's':
			sort(pending, p, 0, p - 1);
			sort(ready, r, 0, r - 1);
			sort(finished, f, 0, f - 1);
			printAll(pending, ready, finished);
			break;
		case '$':
			std::cout << "losses:" << losses << "\t" << " gains:" << gains << "\t" << "profit:" << gains - losses << "\n";// money made today
			break;
		case '0'://secret function to add orders fast for testing
			defaultOrder(pending);//adds a default order to pening for testing
			break;
		default:
			std::cout << "\nsorry thats not an option please try another one\n";
			break;
		}
		system("pause");
		refresh();
		cv.notify_all();
	}

	delete[] pending;
	delete[] ready;
	delete[] finished;

	return 0;
}

void Shef::Movetoready(Order& order, Order*& destination, int& quant, int& size) {
	size++;
	if (size >= quant) increaseSize(destination, quant);
	calculate_cost(order);
	calculate_prep_time(order);
	calcTemp_price(order);
	order.setStatus(DEL);
	if(order.isduplicate()==false) losses += order.get_cost();//add to losses
	destination[size] = order;
	//std::cout << "\norder succesfully moved from pending to ready \n";
}

void Deliveryman::Movetofinished(Order& order, Order*& destination, int& quant, int& size) {
	size++;
	if (size >= quant) increaseSize(destination, quant);

	order.setStatus(FIN);
	calculate_fprice(order);
	destination[size] = order;
	//std::cout << "\norder succesfully moved from ready to finished \n";
}

void refresh() {
	std::cout << "\033[2J\033[H";
}

void options(char& choice) {
	std::cout << "order handling app v3\n";
	std::cout << "=====================\n";
	std::cout << "\rpending:" << p + 1 << " \\ " << "ready:" << r + 1 << " \\ " << "finished:" << f + 1 << "\n";
	std::cout << "\nplease select one of the following options you would like to do\n";
	std::cout << "----------------------------------------------------------------\n";
	std::cout << "x -> closes the program\n";
	std::cout << "+ -> makes a new order\n";
	std::cout << "p -> prints all the pending orders\n";
	std::cout << "d -> prints all the orders that are ready for delivery\n";
	std::cout << "f -> prints all the orders that have been delievered\n";
	std::cout << "= -> prints all the orders with their statuses\n";
	std::cout << "\\ -> prepares an pending order for delivery\n";
	std::cout << "! -> delivers an order and receive payment\n";
	std::cout << "e -> saves current orders to their apropriate files and saves profit too\n";
	std::cout << "i -> loads all past orders from  their apropriate files including profits and sorts them by price\n";
	std::cout << "s -> sorts pending orders by price\n";
	std::cout << "$ ->to get statistics of profits\n";
	std::cout << "@&%&%@&@%&%@&@%@&%@&@%&@%&@%@&%@%@&%@&@%&%@&%@&%%&%@&%@&%@*@%@*@\n";
	std::cout << "select your option:";
	std::cin >> choice;
	std::cin.ignore();
}

void save_profit() {
	std::fstream p;
	double h_gains = 0.0;
	double h_losses = 0.0;
	std::string buffer = "";
	char c = ' ';
	p.open("bank.txt", std::ios::in);
	if (p.is_open()) {
		p.seekg(7);
		while ((c = p.get()) != '\t' && c != EOF) {
			buffer += c;
		}

		h_gains = std::stod(buffer);
		buffer.clear(); // Clear buffer

		p.seekg(6, std::ios_base::cur); // Move file pointer 6 characters from current position

		// Read and store h_losses
		while ((c = p.get()) != '\t' && c != EOF) {
			buffer += c;
		}

		h_losses = std::stod(buffer);
	}
	else std::cout << "\nsomething went wrong\n";
	p.close();// Close file after reading

	p.open("bank.txt", std::ios::out);
	if (p.is_open()) {
		losses += h_losses;
		losses += (delivery_men_size + shefs_size) * (Worker::getDSalary());
		gains += h_gains;
		p << "losses:" << losses << "\t" << "gains:" << gains << "\t" << "profit:" << gains - losses << "\n";
	}
	else std::cout << "\nsomething went wrong\n";
	p.close();// Close file after reading
}

void exportdata(Order* pending, Order* ready, Order* finished) {
	std::ofstream writter;
	writter.open("pending.txt", std::ios::app);
	if (writter.is_open()) {
		for (int i = 0; i <= p; i++) {
			if (!pending[i].isduplicate()) {
				pending[i].setdupped(true);
				writter << "date:" << pending[i].get_date() << " status ->" << pending[i].getstatus() << "\n";
				writter << "name:" << pending[i].get_name() << "\n";
				writter << "adress:" << pending[i].get_adress() << "\n";
				writter << "------products------\n";
				writter << "fries:" << pending[i].fries_included() << "\n";
				for (int j = 0; j < pending[i].getVector().size(); j++) {
					writter << "* " << pending[i].getVector().at(j).getName()
						<< "\t" << "price:" << pending[i].getVector().at(j).getPrice() << "$"
						<< "\t" << "production cost:" << pending[i].getVector().at(j).getCost() << "$"
						<< "\t" << "preparation time:" << pending[i].getVector().at(j).getTime() << " minutes\n";
				}
				writter << "============================================\n";
			}
		}
	}
	else {
		std::cout << "\nsomething went wrong\n";
	}
	writter.close();

	writter.open("ready.txt", std::ios::app);
	if (writter.is_open()) {
		for (int i = 0; i <= r; i++)
		{
			if (!ready[i].isduplicate()) {
				ready[i].setdupped(true);
				writter << "date:" << ready[i].get_date() << " status ->" << ready[i].getstatus() << "\n";
				writter << "name:" << ready[i].get_name() << "\n";
				writter << "adress:" << ready[i].get_adress() << "\n";
				writter << "------products------\n";
				writter << "fries:" << ready[i].fries_included() << "\n";
				for (int j = 0; j < ready[i].getVector().size(); j++) {
					writter << "* " << ready[i].getVector().at(j).getName()
						<< "\t" << "price:" << ready[i].getVector().at(j).getPrice() << "$"
						<< "\t" << "production cost:" << ready[i].getVector().at(j).getCost() << "$"
						<< "\t" << "preparation time:" << ready[i].getVector().at(j).getTime() << " minutes\n";
				}
				writter << "price to make:" << ready[i].get_cost() << "\n";
				writter << "============================================\n";
			}
		}
	}
	else {
		std::cout << "\nsomething went wrong\n";
	}
	writter.close();

	writter.open("delivered.txt", std::ios::app);
	if (writter.is_open()) {
		for (int i = 0; i <= f; i++)
		{
			if (!finished[i].isduplicate()) {
				finished[i].setdupped(true);
				writter << "date:" << finished[i].get_date() << " status ->" << finished[i].getstatus() << "\n";
				writter << "name:" << finished[i].get_name() << "\n";
				writter << "adress:" << finished[i].get_adress() << "\n";
				writter << "------products------\n";
				writter << "fries:" << finished[i].fries_included() << "\n";
				for (int j = 0; j < finished[i].getVector().size(); j++) {
					writter << "* " << finished[i].getVector().at(j).getName()
						<< "\t" << "price:" << finished[i].getVector().at(j).getPrice() << "$"
						<< "\t" << "production cost:" << finished[i].getVector().at(j).getCost() << "$"
						<< "\t" << "preparation time:" << finished[i].getVector().at(j).getTime() << " minutes\n";
				}
				writter << "price to make:" << finished[i].get_cost() << "\n";
				writter << "time it took for delivery:" << finished[i].get_deliveryTime() << "\n";
				writter << "price customer paid:" << finished[i].get_price() << "\n";
				writter << "============================================\n";
			}
		}
	}
	else {
		std::cout << "\nsomething went wrong\n";
	}
	writter.close();
}

void importPend(Order*& pending) {
	std::ifstream reader("pending.txt"); // Open the file directly
	if (!reader.is_open()) {
		std::cerr << "\nSomething went wrong while opening the file\n";
		return;
	}

	std::string buffer = "";
	int day, month, year;
	bool fries;
	std::string address, name, p_name;
	std::vector<Product> foods;
	std::vector<Order> orders;
	double price;
	float p_cost;
	int preparation_time;


	while (reader.peek() != EOF) {
		// Reset foods for the new order
		foods.clear();

		// Skip to date
		reader.ignore(5, ':');
		std::getline(reader, buffer, '\\'); // Read day
		day = std::stoi(buffer);


		std::getline(reader, buffer, '\\'); // Read month
		month = std::stoi(buffer);

		std::getline(reader, buffer, ' '); // Read year
		year = std::stoi(buffer);

		// Read name // Skip the space after ':'
		reader.ignore(16);
		std::getline(reader, name, '\n'); // Read until the end of the line

		// Skip to address
		reader.ignore(7, ':');
		std::getline(reader, address); // Read address

		// Skip to fries status
		reader.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		reader.ignore(7, ':');
		fries = (reader.get() == '1');
		reader.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		// Parse products
		while (reader.peek() == '*') {
			reader.ignore(2); // Skip "* "
			std::getline(reader, p_name, ' ');// Read product name

			p_name += " ";
			std::string p_name2;
			std::getline(reader, p_name2, '\t');
			p_name += p_name2;

			reader.ignore(7, ':');
			reader >> price;

			reader.ignore(std::numeric_limits<std::streamsize>::max(), '$'); // Skip until '$'
			reader.ignore(18, ':');
			reader >> p_cost;

			reader.ignore(std::numeric_limits<std::streamsize>::max(), '$'); // Skip until '$'
			reader.ignore(19, ':');
			reader >> preparation_time;

			reader.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Move to next line

			// Add product to foods vector
			foods.emplace_back(Product(p_name, price, p_cost, preparation_time));
		}


		// Create order and add to vector
		orders.emplace_back(Order(name, address, PEND, foods, day, month, year, fries));

		// Skip delimiter lines or separators if needed
		reader.ignore(std::numeric_limits<std::streamsize>::max(), '=');
		reader.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	add_order(pending, orders, p, p_size);
}

void add_order(Order*& order, std::vector<Order>& new_, int& track, int& sizeof_array) {
	size_t size_new = new_.size();
	for (int i = 0; i < size_new; i++) {
		track++;
		if (track >= sizeof_array) increaseSize(order, sizeof_array);
		order[track] = new_.at(i);
		order[track].setdupped(true);
		calculate_prep_time(order[track]);
	}
	//cv.notify_all();
}

void importReady(Order*& ready)
{
	std::ifstream reader("ready.txt"); // Open the file directly
	if (!reader.is_open()) {
		std::cerr << "\nSomething went wrong while opening the file\n";
		return;
	}

	std::string buffer = "";
	int day, month, year;
	bool fries;
	std::string address, name, p_name;
	std::vector<Product> foods;
	std::vector<Order> orders;
	double price;
	float p_cost;
	int preparation_time;

	int i = -1;
	while (reader.peek() != EOF) {
		// Reset foods for the new order
		i++;
		foods.clear();

		// Skip to date
		reader.ignore(5, ':');
		std::getline(reader, buffer, '\\'); // Read day
		day = std::stoi(buffer);


		std::getline(reader, buffer, '\\'); // Read month
		month = std::stoi(buffer);

		std::getline(reader, buffer, ' '); // Read year
		year = std::stoi(buffer);

		// Read name // Skip the space after ':'
		reader.ignore(16);
		std::getline(reader, name, '\n'); // Read until the end of the line

		// Skip to address
		reader.ignore(7, ':');
		std::getline(reader, address); // Read address

		// Skip to fries status
		reader.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		reader.ignore(7, ':');
		fries = (reader.get() == '1');
		reader.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		// Parse products
		while (reader.peek() == '*') {

			reader.ignore(2); // Skip "* "
			std::getline(reader, p_name, ' ');// Read product name

			p_name += " ";
			std::string p_name2;
			std::getline(reader, p_name2, '\t');
			p_name += p_name2;

			reader.ignore(7, ':');
			reader >> price;

			reader.ignore(std::numeric_limits<std::streamsize>::max(), '$'); // Skip until '$'
			reader.ignore(18, ':');
			reader >> p_cost;

			reader.ignore(std::numeric_limits<std::streamsize>::max(), '$'); // Skip until '$'
			reader.ignore(19, ':');
			reader >> preparation_time;

			reader.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Move to next line

			// Add product to foods vector
			foods.emplace_back(Product(p_name, price, p_cost, preparation_time));
		}


		// Create order and add to vector
		orders.emplace_back(Order(name, address, DEL, foods, day, month, year, fries));
		calculate_cost(orders.at(i));
		calculate_prep_time(orders.at(i));
		calcTemp_price(orders.at(i));
		//losses += orders.at(i).get_cost();

		// Skip delimiter lines or separators if needed
		reader.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		reader.ignore(std::numeric_limits<std::streamsize>::max(), '=');
		reader.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	add_order(ready, orders, r, r_size);
}

void importFin(Order*& finished) {
	std::ifstream reader("delivered.txt"); // Open the file directly
	if (!reader.is_open()) {
		std::cerr << "\nSomething went wrong while opening the file\n";
		return;
	}

	std::string buffer = "";
	int day, month, year;
	bool fries;
	std::string address, name, p_name;
	std::vector<Product> foods;
	std::vector<Order> orders;
	double price;
	float p_cost;
	int preparation_time;

	int i = -1;
	int delivery_time;
	while (reader.peek() != EOF) {
		// Reset foods for the new order
		foods.clear();
		i++;
		// Skip to date
		reader.ignore(5, ':');
		std::getline(reader, buffer, '\\'); // Read day
		day = std::stoi(buffer);


		std::getline(reader, buffer, '\\'); // Read month
		month = std::stoi(buffer);

		std::getline(reader, buffer, ' '); // Read year
		year = std::stoi(buffer);

		// Read name // Skip the space after ':'
		reader.ignore(16);
		std::getline(reader, name, '\n'); // Read until the end of the line

		// Skip to address
		reader.ignore(7, ':');
		std::getline(reader, address); // Read address

		// Skip to fries status
		reader.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		reader.ignore(7, ':');
		fries = (reader.get() == '1');
		reader.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		// Parse products
		while (reader.peek() == '*') {
			reader.ignore(2); // Skip "* "
			std::getline(reader, p_name, ' ');// Read product name

			p_name += " ";
			std::string p_name2;
			std::getline(reader, p_name2, '\t');
			p_name += p_name2;

			reader.ignore(7, ':');
			reader >> price;

			reader.ignore(std::numeric_limits<std::streamsize>::max(), '$'); // Skip until '$'
			reader.ignore(18, ':');
			reader >> p_cost;

			reader.ignore(std::numeric_limits<std::streamsize>::max(), '$'); // Skip until '$'
			reader.ignore(19, ':');
			reader >> preparation_time;

			reader.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Move to next line

			// Add product to foods vector
			foods.emplace_back(Product(p_name, price, p_cost, preparation_time));
		}


		// Create order and add to vector
		orders.emplace_back(Order(name, address, FIN, foods, day, month, year, fries));

		// Skip delimiter lines or separators if needed
		reader.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		reader.ignore(std::numeric_limits<std::streamsize>::max(), ':');
		reader >> delivery_time;
		orders.at(i).setDelivery_time(delivery_time);
		calculate_cost(orders.at(i));
		calculate_prep_time(orders.at(i));
		calcTemp_price(orders.at(i));
		calculate_fprice(orders.at(i));
		reader.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		reader.ignore(std::numeric_limits<std::streamsize>::max(), '=');
		reader.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	add_order(finished, orders, f, f_size);
}

void calculate_fprice(Order& order) {
	//formula price=servicefee+price_of_travel+(price_of_foods)*10%off
	size_t size = order.getVector().size();
	double price = 0.0;
	for (int i = 0; i < size; i++) {
		price += order.getVector().at(i).getPrice();
	}


	auto temp = price;
	if (size >= 3) {
		price = temp - price * (0.1);
	}

	price += SERVICE_FEE;
	price += calc_travel_fee(order);
	order.setPrice(price);
	if(order.isduplicate()==false)
	gains += price;
}

double calc_travel_fee(Order& order) {
	int times = order.get_deliveryTime();
	times /= 5;
	return times * DISTANCE_FEE;
}

void calculate_cost(Order& order) {
	size_t size = order.getVector().size();
	float cost = 0.0;
	for (int i = 0; i < size; i++) {
		cost += order.getVector().at(i).getCost();
	}
	order.setCost(cost);
}

void calculate_prep_time(Order& order) {
	size_t size = order.getVector().size();
	float prep_time = 0.0;
	for (int i = 0; i < size; i++) {
		prep_time += order.getVector().at(i).getTime();
	}
	order.setprep_time(prep_time);
}

void increaseSize(Order*& array, int& current_size) {
	int ogsize = current_size;
	current_size *= 2;
	Order* arr = new(std::nothrow) Order[current_size];

	if (!arr) {
		std::cerr << "Memory allocation failed during resize.\n";
		return;
	}

	for (int i = 0; i < ogsize; i++)  arr[i] = array[i];

	delete[] array;
	array = arr;
}

void defaultOrder(Order*& pending) {
	std::string name = "theo adams";
	std::string adress = "ousani 6";
	std::string meal = "pork gyros";
	Product* food = new Product[3];
	for (int i = 0; i < 3; i++) {
		food[i] = Product(meal, 3.2, GYROS, EASY);
	}

	p++;
	if ((p + 1) == p_size) {
		increaseSize(pending, p_size);
	}
	pending[p] = Order(name, adress, PEND, food, 3, 28, 8, 2024, true);

	delete[] food;
	std::cout << "\nsuccesfully added default order\n";
	//cv.notify_all();
}

void toready(Order*& og, Order*& new_) {
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
		calculate_cost(new_[r]);
		//calculate_prep_time(new_[r]);
		calcTemp_price(new_[r]);
		new_[r].setdupped(false);
		if(new_[num].isduplicate()==false)
		losses += new_[num].get_cost();//add to losses
		std::cout << "\norder succesfully moved from pending to ready \n";
	}
	else {
		std::cout << "\nout of bounds\n";
	}
}

void tofinished(Order* og, Order* new_) {
	print(og, r, DEL);
	std::cout << "\nselect one of the above orders to be prepared to deliver by typing its order number:";
	int num;
	std::cin >> num;
	//check if number exceeds the ready orders size
	num--;
	if (num >= 0 && num <= r) {
		f++;
		if ((f + 1) == f_size) increaseSize(new_, f_size);
		new_[f] = og[num];
		new_[f].setStatus(FIN);
		calculate_fprice(new_[f]);
		new_[f].setdupped(false);
		og[num] = og[r];
		r--;//reduce the number of elemnts in the original array
		std::cout << "\norder succesfully moved from ready to finished \n";
	}
	else {
		std::cout << "\nout of bounds\n";
	}
}

void calcTemp_price(Order& new_) {
	size_t size = new_.getVector().size();
	double price = 0;
	for (int i = 0; i < size; i++) {
		price += new_.getVector().at(i).getPrice();
	}
	//fries fee
	if (new_.fries_included()) price += 2.5;
	new_.setPrice(price);
}

void printAll(Order*& pend, Order*& ready, Order*& fin) {
	print(pend, p, PEND);
	print(ready, r, DEL);
	print(fin, f, FIN);
}

void  print(Order*& data, int quantity, int status)
{
	if (quantity >= 0) {
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
				std::cout << "price:" << data[i].get_price() << " $ " << "time to prepare the order:" << data[i].get_preparationTime() << " minutes\n";
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
				std::cout << "delivery time:" << data[i].get_deliveryTime() << " construction cost:" << data[i].get_cost() << " $\n";
			}
			std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++\n";
			std::cout << "----above you have all the delivered orders----\n";
			break;
		}

	}
	else std::cout << "\n=========no available orders=========\n";
}

void add_order(Order*& order) {
	std::cout << "\nenter your name:";
	std::string name;
	std::getline(std::cin >> std::ws, name);
	std::cout << "enter your adress:";
	std::string adress;
	std::getline(std::cin >> std::ws, adress);
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
			product_name = "pork gyros";
			dificulty = EASY;
			price = 3.20;
			cost = GYROS;
			break;
		case '2':
			product_name = "chicken gyros";
			dificulty = EASY;
			price = 3.2;
			cost = GYROS;
			break;
		case '3':
			product_name = "cheeseburger";
			dificulty = HARD;
			price = 6;
			cost = CHEESE_BURGER;
			break;
		case '4':
			product_name = "chicken wings";
			dificulty = MEDIUM;
			price = 5.10;
			cost = WINGS;
			break;
		case '5':
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
	std::cout << "\nhow many servings would you like:";
	std::cin >> quantity;

	std::string answer;
	do {
		std::cout << "\nwant fries with that(y\\n):";
		std::cin >> answer;
		fflush(stdin);
	} while (answer != "n" && answer != "y");

	(answer == "y") ? fries = true : fries = false;

	Product* food = new Product[quantity];

	for (int i = 0; i < quantity; i++) {
		food[i] = Product(product_name, price, cost, dificulty);
	}

	auto now = std::chrono::system_clock::now();

	std::time_t now_c = std::chrono::system_clock::to_time_t(now);

	std::tm now_tm;

	if (localtime_s(&now_tm, &now_c) != 0) {
		std::cerr << "Error converting time." << std::endl;
		return;
	}

	p++;
	//resize when out of memmory
	if ((p + 1) == p_size) increaseSize(order, p_size);

	order[p] = Order(name, adress, PEND, food, quantity, now_tm.tm_mday, now_tm.tm_mon + 1, now_tm.tm_year + 1900, fries);
	calculate_prep_time(order[p]);
	delete[] food;
	cv.notify_all();
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

void sort(Order*& array, int size, int start, int end) {
	if (size == TOO_SMALL) insertionSort(array, size);
	else quickSort(array, start, end);
}

void insertionSort(Order*& array, int size) {
	Order temp;
	int offset;
	for (int i = 0; i < size; i++) {
		temp = array[i];
		offset = i - 1;
		while (offset > -1 && array[offset].get_price() > temp.get_price()) {
			array[offset + 1] = array[offset];
			offset--;
		}
		array[++offset] = temp;
	}
}

void quickSort(Order*& array, int start, int end) {
	if (start >= end) return;
	int pivot = start - 1, j = start;
	Order temp;

	while (j < end) {

		if (array[j].get_price() < array[end].get_price()) {
			temp = array[++pivot];
			array[pivot] = array[j];
			array[j++] = temp;
		}
		else j++;
	}

	pivot++;
	temp = array[pivot];
	array[pivot] = array[end];
	array[end] = temp;
	quickSort(array, start, pivot - 1);//left array
	quickSort(array, pivot + 1, end);//right array
}