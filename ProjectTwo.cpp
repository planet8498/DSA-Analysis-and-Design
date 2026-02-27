// ProjectTwo.cpp
// ABCU Advising Assistance Program (CS 300 Project Two)
// Single-file implementation using a custom Binary Search Tree (BST)

#include<cctype>
#include<fstream>
#include<iostream>
#include<sstream>
#include<string>
#include<vector>

using namespace std;

struct Course {
	string number;                                // e.g., "CSCI300"
	string title;                                 // e.g., "Introduction to Algorithms"
	vector<string> prerequisites;                 // e.g., ["CSCI200", "MATH201"]
};

// Utility Helpers
static inline string trim(const string& s) {
	size_t start = 0;
	while (start < s.size() && isspace(static_cast<unsigned char>(s[start]))) start++;
	size_t end = s.size();
	while (end > start && isspace(static_cast<unsigned char>(s[end - 1]))) end--;
	return s.substr(start, end - start);
}

static inline string toUpper(string s) {
	for (char& ch : s) ch = static_cast<char>(toupper(static_cast<unsigned char>(ch)));
	return s;
}

// Simple CSV split
static vector<string> splitCSVLine(const string& line) {
	vector<string> fields;
	string field;
	stringstream ss(line);
	while (getline(ss, field, ',')) fields.push_back(trim(field));
	return fields;
}

// BST Implementation
class CourseBST {
private:
	struct Node {
		Course course;
		Node* left;
		Node* right;
		Node(const Course& c) : course(c), left(nullptr), right(nullptr){}
	};

	Node* root = nullptr;

	void destroy(Node* node) {
		if (!node) return;
		destroy(node->left);
		destroy(node->right);
		delete node;
	}

	Node* insert(Node* node, const Course& course) {
		if (!node) return new Node(course);

		if (course.number < node->course.number) {
			node->left = insert(node->left, course);
		} else if (course.number > node->course.number) {
			node->right = insert(node->right, course);
		} else {
			// Duplicate key: replace existing course data
			node->course = course;
		}
		return node;
	}

	const Course* search(Node* node, const string& courseNum) const {
		if (!node) return nullptr;

		if(courseNum == node->course.number) return &node->course;
		if(courseNum < node->course.number) return search(node->left, courseNum);
		return search(node->right, courseNum);
	}

	void inOrderPrint(Node* node) const {
		if (!node) return;
		inOrderPrint(node->left);
		cout << node->course.number << "," << node->course.title << endl;
		inOrderPrint(node->right);
	}

public:
	~CourseBST() { clear(); }

	void clear() {
		destroy(root);
		root = nullptr;
	}

	void insert(const Course& course) {
		root = insert(root, course);
	}

	const Course* search(const string& courseNum) const {
		return search(root, courseNum);
	}

	void printInOrder() const {
		inOrderPrint(root);
	}

	bool empty() const {
		return root == nullptr;
	}
};

// Program Functions
static void printMenu() {
	cout << "1. Load Data Structure." << endl;
	cout << "2. Print Course List." << endl;
	cout << "3. Print Course." << endl;
	cout << "9. Exit" << endl;
}

static bool loadCoursesFromFile(const string& filename, CourseBST& bst) {
	ifstream file(filename);
	if (!file.is_open()) return false;

	bst.clear();

	string line;
	while (getline(file, line)) {
		line = trim(line);
		if (line.empty()) continue;

		vector<string> fields = splitCSVLine(line);
		if (fields.size() < 2) continue; //malformed line

		Course c;
		c.number = toUpper(fields[0]);
		c.title = fields[1];

		for (size_t i = 2; i < fields.size(); i++) {
			string prereq = toUpper(fields[i]);
			if (!prereq.empty()) c.prerequisites.push_back(prereq);
		}

		bst.insert(c);
	}

	return true;

}

static void printCourseList(const CourseBST& bst) {
	cout << "Here is a sample schedule:" << endl;
	bst.printInOrder(); // in-order traversal gives alphanumeric order
}

static void printSingleCourse(const CourseBST& bst) {
	cout << "What course do you want to know about?";
	string input;
	getline(cin, input);

	string courseNum = toUpper(trim(input));
	const Course* c = bst.search(courseNum);

	if  (!c) {
		cout << "Course not found: " << courseNum << endl;
		return;
	}

	cout << c->number << "," << c->title << endl;

	if (c->prerequisites.empty()) {
		cout << "Prerequisites: None" << endl;
		return;

	}

	cout << "Prerequisites: ";
	for (size_t i = 0; i < c->prerequisites.size(); i++) {
		const string& pnum = c->prerequisites[i];
		cout << pnum;

		// If prereq course exists, show its title too
		const Course* prereqCourse = bst.search(pnum);
		if (prereqCourse) {
			cout << "(" << prereqCourse->title << ")";
		}

		if (i + 1 < c->prerequisites.size()) cout << ",";
	}
	cout << endl;
}

// Main
int main() {
	cout << "Welcome to the course planner." << endl;

	CourseBST bst;
	bool loaded = false;

	while (true) {
		printMenu();
		cout << "What would you like to do?";

		string choiceLine;
		getline(cin, choiceLine);
		choiceLine = trim(choiceLine);

		int choice = -1;
		try {
			choice = stoi(choiceLine);
		} catch (...) {
			cout << choiceLine << "is not a valid option." << endl;
			continue;
		}

		if (choice == 1) {
			cout << "Enter the file name:";
			string filename;
			getline(cin, filename);
			filename = trim(filename);

			if (filename.empty()) {
				cout << "Error: file name cannot be empty." << endl;
				loaded = false;
				continue;
			}

			if (loadCoursesFromFile(filename, bst)) {
				loaded = true;
				cout << "File loaded successfully." << endl;
			} else {
				loaded = false;
				cout << "Error: could not open file \"" << filename << "\"." << endl;
			}

		}else if (choice == 2) {
			if (!loaded || bst.empty()) {
				cout << "Error; Please load data first (Option 1)." << endl;
				continue;
			}
			printCourseList(bst);

		} else if (choice == 3) {
			if (!loaded || bst.empty()) {
				cout << "Error: Please load data first (Option 1)." << endl;
				continue;
			}
			printSingleCourse(bst);
		} else if (choice == 9) {
			cout << "Thank you for using the course planner!" << endl;
			break;

		} else {
			cout << choice << "is not a valid option." << endl;
		}
	}

	return 0;
}
