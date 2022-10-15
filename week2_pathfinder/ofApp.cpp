/*

	ofxWinMenu basic example - ofApp.cpp

	Example of using ofxWinMenu addon to create a menu for a Microsoft Windows application.
	
	Copyright (C) 2016-2017 Lynn Jarvis.

	https://github.com/leadedge

	http://www.spout.zeal.co

    =========================================================================
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    =========================================================================

	03.11.16 - minor comment cleanup
	21.02.17 - rebuild for OF 0.9.8

*/
#include "ofApp.h"
#include <iostream>
using namespace std;

#define max_stack_size 1000000
#define max_queue_size 1000000
int top = -1;
int front = -1; int rear = -1;

enum direction { right_dir, down_dir, left_dir, up_dir };

typedef struct {
	int row;
	int col;
	bool right;
	bool down;
	enum direction came_from;
} vertex;



vertex** cell;
vertex mystack[max_stack_size];
vertex myqueue[max_queue_size];

bool dfs_done = false;

void stack_full() {
	printf("\nError: stack is full\n");
	exit(0);
}

bool stack_isempty() {
	if (top < 0) return true;
	return false;
}

bool stack_isfull() {
	if (top >= max_stack_size - 1) return true;
	return false;
}

void push(vertex room) {
	if (stack_isfull()) stack_full();
	mystack[++top] = room;
}

vertex pop() {
	if (stack_isempty()) {
		printf("Error: stack is empty\n");
		exit(0);
	}
	return mystack[top--];
}

void queue_full() {
	printf("\nError: stack is full\n");
	exit(0);
}

bool queue_isempty() {
	if (front == rear) return true;
	return false;
}

bool queue_isfull() {
	if (rear >= max_queue_size - 1) return true;
	return false;
}

void addq(vertex room) {
	if (queue_isfull()) queue_full();
	myqueue[++rear] = room;
}

vertex deleteq() {
	if (queue_isempty()) {
		printf("Error: stack is empty\n");
		exit(0);
	}
	return myqueue[++front];
}

float startx = 20; float starty = 20;

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetWindowTitle("Maze Example"); // Set the app name on the title bar
	ofSetFrameRate(15);
	ofBackground(255, 255, 255);
	// Get the window size for image loading
	windowWidth = ofGetWidth();
	windowHeight = ofGetHeight();
	isdfs = false;
	isbfs = false;
	isOpen = 0;
	// Centre on the screen
	ofSetWindowPosition((ofGetScreenWidth()-windowWidth)/2, (ofGetScreenHeight()-windowHeight)/2);

	// Load a font rather than the default
	myFont.loadFont("verdana.ttf", 12, true, true);

	// Load an image for the example
	//myImage.loadImage("lighthouse.jpg");

	// Window handle used for topmost function
	hWnd = WindowFromDC(wglGetCurrentDC());

	// Disable escape key exit so we can exit fullscreen with Escape (see keyPressed)
	ofSetEscapeQuitsApp(false);

	//
	// Create a menu using ofxWinMenu
	//

	// A new menu object with a pointer to this class
	menu = new ofxWinMenu(this, hWnd);

	// Register an ofApp function that is called when a menu item is selected.
	// The function can be called anything but must exist. 
	// See the example "appMenuFunction".
	menu->CreateMenuFunction(&ofApp::appMenuFunction);

	// Create a window menu
	HMENU hMenu = menu->CreateWindowMenu();

	//
	// Create a "File" popup menu
	//
	HMENU hPopup = menu->AddPopupMenu(hMenu, "File");

	//
	// Add popup items to the File menu
	//

	// Open an maze file
	menu->AddPopupItem(hPopup, "Open", false, false); // Not checked and not auto-checked
	
	// Final File popup menu item is "Exit" - add a separator before it
	menu->AddPopupSeparator(hPopup);
	menu->AddPopupItem(hPopup, "Exit", false, false);

	//
	// View popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "View");

	bShowInfo = true;  // screen info display on
	menu->AddPopupItem(hPopup, "Show DFS",false,false); // Checked
	bTopmost = false; // app is topmost
	menu->AddPopupItem(hPopup, "Show BFS"); // Not checked (default)
	bFullscreen = false; // not fullscreen yet
	menu->AddPopupItem(hPopup, "Full screen", false, false); // Not checked and not auto-check

	//
	// Help popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "Help");
	menu->AddPopupItem(hPopup, "About", false, false); // No auto check

	// Set the menu to the window
	menu->SetWindowMenu();

} // end Setup


//
// Menu function
//
// This function is called by ofxWinMenu when an item is selected.
// The the title and state can be checked for required action.
// 
void ofApp::appMenuFunction(string title, bool bChecked) {

	ofFileDialogResult result;
	string filePath;
	size_t pos;

	//
	// File menu
	//
	if(title == "Open") {
		readFile();
	}
	if(title == "Exit") {
		ofExit(); // Quit the application
	}

	//
	// Window menu
	//
	if(title == "Show DFS") {
		//bShowInfo = bChecked;  // Flag is used elsewhere in Draw()
		isdfs = true;
		isbfs = false;
		if (isOpen)
		{
			//DFS();
			bShowInfo = bChecked;
		}
		else
			cout << "you must open file first" << endl;
		
	}

	if(title == "Show BFS") {
		doTopmost(bChecked); // Use the checked value directly
		isbfs = true;
		isdfs = false;
		if (isOpen) {
			bShowInfo = bChecked;
		}
		else
			cout << "you must open file first" << endl;
	}

	if(title == "Full screen") {
		bFullscreen = !bFullscreen; // Not auto-checked and also used in the keyPressed function
		doFullScreen(bFullscreen); // But als take action immediately
	}

	//
	// Help menu
	//
	if(title == "About") {
		ofSystemAlertDialog("ofxWinMenu\nbasic example\n\nhttp://spout.zeal.co");
	}

} // end appMenuFunction


//--------------------------------------------------------------
void ofApp::update() {

}


//--------------------------------------------------------------
void ofApp::draw() {

	char str[256];
	//ofBackground(0, 0, 0, 0);
	ofSetColor(100);
	ofSetLineWidth(5);
	int i, j;

	float linex, liney;
	// TO DO : DRAW MAZE; 
	// 저장된 자료구조를 이용해 미로를 그린다.
	// add code here

	for (i = 0; i < HEIGHT; i++) {
		if (i == 0) // 맨 윗줄 그리기
			ofDrawLine(startx, starty, startx + 40.0f * WIDTH, starty);
		for (j = 0; j < WIDTH; j++) {
			if (j == 0) { // 맨 왼쪽 벽 그리기
				linex = startx;
				liney = starty + 40.0f * (float)i;
				ofDrawLine(linex, liney, linex, liney + 40.0f);
			}
			if (cell[i][j].right == 0) { // 해당 칸의 오른쪽 벽 그리기
				linex = startx + 40.0f * ((float)j + 1.0f);
				liney = starty + 40.0f * (float)i;
				ofDrawLine(linex, liney, linex, liney + 40.0f);
			}
			if (cell[i][j].down == 0) { // 해당 칸의 아랫쪽 벽 그리기
				linex = startx + 40.0f * (float)j;
				liney = starty + 40.0f * ((float)i + 1.0f);
				ofDrawLine(linex, liney, linex + 40.0f, liney);
			}
		}
	}

	if (isdfs)
	{
		ofSetColor(200);
		ofSetLineWidth(5);
		if (isOpen)
			//dfsdraw();
		{
			DFS();
			while (!stack_isempty()) pop(); // stack 초기화
			for (i = 0; i < HEIGHT; i++) { // visited 초기화
				for (j = 0; j < WIDTH; j++) {
					visited[i][j] = 0;
				}
			}
		}
		else
			cout << "You must open file first" << endl;
	}
	else if (isbfs) {
		ofSetColor(200);
		ofSetLineWidth(5);
		if (isOpen)
		{
			BFS();
			while (!queue_isempty()) deleteq(); // queue 초기화
			for (i = 0; i < HEIGHT; i++) { // visited 초기화
				for (j = 0; j < WIDTH; j++) {
					visited[i][j] = 0;
				}
			}
		}
		else
			cout << "You must open file first" << endl;
	}
	if(bShowInfo) {
		// Show keyboard duplicates of menu functions
		sprintf(str, " comsil project");
		myFont.drawString(str, 15, ofGetHeight()-20);
	}

} // end Draw


void ofApp::doFullScreen(bool bFull)
{
	// Enter full screen
	if(bFull) {
		// Remove the menu but don't destroy it
		menu->RemoveWindowMenu();
		// hide the cursor
		ofHideCursor();
		// Set full screen
		ofSetFullscreen(true);
	}
	else { 
		// return from full screen
		ofSetFullscreen(false);
		// Restore the menu
		menu->SetWindowMenu();
		// Restore the window size allowing for the menu
		ofSetWindowShape(windowWidth, windowHeight + GetSystemMetrics(SM_CYMENU)); 
		// Centre on the screen
		ofSetWindowPosition((ofGetScreenWidth()-ofGetWidth())/2, (ofGetScreenHeight()-ofGetHeight())/2);
		// Show the cursor again
		ofShowCursor();
		// Restore topmost state
		if(bTopmost) doTopmost(true);
	}

} // end doFullScreen


void ofApp::doTopmost(bool bTop)
{
	if(bTop) {
		// get the current top window for return
		hWndForeground = GetForegroundWindow();
		// Set this window topmost
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
		ShowWindow(hWnd, SW_SHOW);
	}
	else {
		SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(hWnd, SW_SHOW);
		// Reset the window that was topmost before
		if(GetWindowLong(hWndForeground, GWL_EXSTYLE) & WS_EX_TOPMOST)
			SetWindowPos(hWndForeground, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
		else
			SetWindowPos(hWndForeground, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
	}
} // end doTopmost


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	
	// Escape key exit has been disabled but it can be checked here
	if(key == VK_ESCAPE) {
		// Disable fullscreen set, otherwise quit the application as usual
		if(bFullscreen) {
			bFullscreen = false;
			doFullScreen(false);
		}
		else {
			ofExit();
		}
	}

	// Remove or show screen info
	if(key == ' ') {
		bShowInfo = !bShowInfo;
		// Update the menu check mark because the item state has been changed here
		menu->SetPopupItem("Show DFS", bShowInfo);
	}

	if(key == 'f') {
		bFullscreen = !bFullscreen;	
		doFullScreen(bFullscreen);
		// Do not check this menu item
		// If there is no menu when you call the SetPopupItem function it will crash
	}

} // end keyPressed

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
bool ofApp::readFile()
{
	ofFileDialogResult openFileResult = ofSystemLoadDialog("Select .maz file");
	string filePath;
	size_t pos;
	char tempchar;
	int i, j;
	// Check whether the user opened a file
	if (openFileResult.bSuccess) {
		ofLogVerbose("User selected a file");

		//We have a file, check it and process it
		string fileName = openFileResult.getName();
		//string fileName = "maze0.maz";
		printf("file name is %s\n", fileName);
		filePath = openFileResult.getPath();
		printf("Open\n");
		pos = filePath.find_last_of(".");
		if (pos != string::npos && pos != 0 && filePath.substr(pos + 1) == "maz") {

			ofFile file(fileName);
			if (!file.exists()) {
				cout << "Target file does not exists." << endl;
				return false;
			}
			else {
				cout << "We found the target file." << endl;
				isOpen = 1;
			}

			//ofBuffer buffer(file);
			//ofBuffer ofBufferFromFile(file, false);
			//file.open(fileName);
			

			// Input_flag is a variable for indication the type of input.
			// If input_flag is zero, then work of line input is progress.
			// If input_flag is one, then work of dot input is progress.
			//int input_flag = 0;

			// Idx is a variable for index of array.
			int idx = 0;

			// Read file line by line
			HEIGHT = 0; WIDTH = 0;
			
			
			// TO DO
			// .maz 파일을 input으로 받아서 적절히 자료구조에 넣는다
			while (1) { // width 측정
				file.get(tempchar);
				if (tempchar == '\n') break;
				//printf("%c", tempchar);
				if (tempchar == '-') WIDTH++;
			}
			int space_check = 0;
			while (1) { // height 측정
				if (file.eof()) break;
				file.get(tempchar);
				//printf("%c", tempchar);
				if (tempchar == '\n') {
					space_check++;
					if (space_check == 2) {
						HEIGHT++;
						space_check = 0;
					}
				}
			}
			if (++space_check == 2) HEIGHT++; // 엔터로 끝나지 않는 경우를 생각함
			file.close();
			// 메모리 할당
			cell = new vertex * [HEIGHT];
			for (i = 0; i < HEIGHT; i++) {
				cell[i] = new vertex[WIDTH];
			}

			visited = new int* [HEIGHT];
			for (i = 0; i < HEIGHT; i++) {
				visited[i] = new int[WIDTH];
				for (j = 0; j < WIDTH; j++)
					visited[i][j] = 0;
			}
			
			// 자료구조 저장
			file.open(fileName);
			while (1) {
				file.get(tempchar);
				if (tempchar == '\n') break;
			}
			for (i = 0; i < HEIGHT; i++) {
				file.get(tempchar); // 맨왼쪽 벽 읽음
				for (j = 0; j < WIDTH; j++) {
					cell[i][j].row = i;
					cell[i][j].col = j;
					file.get(tempchar); // 방 읽음
					file.get(tempchar); // 방 오른쪽 벽 읽음
					if (tempchar == '|') cell[i][j].right = 0;
					else if (tempchar == ' ') cell[i][j].right = 1;
				}
				file.get(tempchar); // 엔터 읽음
				if (tempchar == '\r') file.get(tempchar);
				file.get(tempchar); // 맨왼쪽 벽 읽음
				for (j = 0; j < WIDTH; j++) {
					file.get(tempchar); // 방과 아랫방 사이 읽음
					if (tempchar == '-') cell[i][j].down = 0;
					else if (tempchar == ' ') cell[i][j].down = 1;
					file.get(tempchar); // 방 모서리 읽음(+)
					//printf("\n%d %d", cell[i][j].right, cell[i][j].down);
				}
				file.get(tempchar); // 엔터 읽음
				if (tempchar == '\r') file.get(tempchar);
				//printf("\n-------------------");
			}
			file.close();
		}
		else {
			printf("  Needs a '.maz' extension\n");
			return false;
		}
	}
	return true;
}
void ofApp::freeMemory() {
	int i;
	//TO DO
	// malloc한 memory를 free해주는 함수
	for (i = 0; i < HEIGHT; i++) {
		delete[] cell[i];
	}
	delete[] cell;
	for (i = 0; i < HEIGHT; i++) {
		delete[] visited[i];
	}
	delete[] visited;
	HEIGHT = 0; WIDTH = 0;
}

bool ofApp::DFS()//DFS탐색을 하는 함수
{
	vertex current;
	float linex, liney, backx, backy;
	//TO DO
	//DFS탐색을 하는 함수 ( 3주차)
	push(cell[0][0]);
	visited[0][0] = 1;
	current = mystack[top];
	ofSetLineWidth(5.0f);
	while (!stack_isempty()) {
		if (current.row == HEIGHT - 1 && current.col == WIDTH - 1)
			return true;
		ofSetColor(ofColor::green);
		linex = startx + 20.0f + ((float)current.col * 40.0f);
		liney = starty + 20.0f + ((float)current.row * 40.0f);
		if (current.right && !visited[current.row][current.col + 1]) { // 오른쪽으로 이동 가능
			current = cell[current.row][current.col + 1];
			push(current);
			visited[current.row][current.col] = 1;
			ofDrawLine(linex, liney, linex + 40.0f, liney);
		}
		else if (current.down && !visited[current.row + 1][current.col]) { // 아랫쪽으로 이동 가능
			current = cell[current.row + 1][current.col];
			push(current);
			visited[current.row][current.col] = 1;
			ofDrawLine(linex, liney, linex, liney + 40.0f);
		}
		else if (current.col > 0 && cell[current.row][current.col - 1].right && !visited[current.row][current.col - 1]) { // 왼쪽으로 이동 가능
			current = cell[current.row][current.col - 1];
			push(current);
			visited[current.row][current.col] = 1;
			ofDrawLine(linex, liney, linex - 40.0f, liney);
		}
		else if (current.row > 0 && cell[current.row - 1][current.col].down && !visited[current.row - 1][current.col]) { // 윗쪽으로 이동 가능
			current = cell[current.row - 1][current.col];
			push(current);
			visited[current.row][current.col] = 1;
			ofDrawLine(linex, liney, linex, liney - 40.0f);
		}
		else {
			ofSetColor(ofColor::red);
			pop();
			if (stack_isempty()) break;
			current = mystack[top];
			backx = startx + 20.0f + ((float)current.col * 40.0f);
			backy = starty + 20.0f + ((float)current.row * 40.0f);
			ofDrawLine(linex, liney, backx, backy);
		}
	}
	return false;
}
bool ofApp::BFS()
{
	//TO DO 
	//BFS를 수행한 결과를 그린다. (3주차 내용)
	vertex current;
	float linex, liney;
	addq(cell[0][0]);
	visited[0][0] = 1;
	ofSetLineWidth(5.0f);
	while (!queue_isempty()) {
		current = deleteq();
		ofSetColor(ofColor::red);
		linex = startx + 20.0f + ((float)current.col * 40.0f);
		liney = starty + 20.0f + ((float)current.row * 40.0f);
		if (current.row == HEIGHT - 1 && current.col == WIDTH - 1) {
			ofSetColor(ofColor::green);
			while (current.row != 0 || current.col != 0) { // 찾은 루트 되감아가며 초록색으로 표시
				switch (cell[current.row][current.col].came_from) {
				case left_dir:
					ofDrawLine(linex, liney, linex - 40.0f, liney);
					current.col--;
					break;
				case up_dir:
					ofDrawLine(linex, liney, linex, liney - 40.0f);
					current.row--;
					break;
				case right_dir:
					ofDrawLine(linex, liney, linex + 40.0f, liney);
					current.col++;
					break;
				case down_dir:
					ofDrawLine(linex, liney, linex, liney + 40.0f);
					current.row++;
					break;
				}
				linex = startx + 20.0f + ((float)current.col * 40.0f);
				liney = starty + 20.0f + ((float)current.row * 40.0f);
			}
			return true; 
		}
		if (current.right && !visited[current.row][current.col + 1]) { // 오른쪽으로 갈 수 있는 경우
			cell[current.row][current.col + 1].came_from = left_dir;
			addq(cell[current.row][current.col + 1]);
			visited[current.row][current.col + 1] = 1;
			ofDrawLine(linex, liney, linex + 40.0f, liney);
		}
		if (current.down && !visited[current.row + 1][current.col]) { // 아랫쪽으로 갈 수 있는 경우
			cell[current.row + 1][current.col].came_from = up_dir;
			addq(cell[current.row + 1][current.col]);
			visited[current.row + 1][current.col] = 1;
			ofDrawLine(linex, liney, linex, liney + 40.0f);
		}
		if (current.col > 0 && cell[current.row][current.col - 1].right && !visited[current.row][current.col - 1]) { // 왼쪽으로 갈 수 있는 경우
			cell[current.row][current.col - 1].came_from = right_dir;
			addq(cell[current.row][current.col - 1]);
			visited[current.row][current.col - 1] = 1;
			ofDrawLine(linex, liney, linex - 40.0f, liney);
		}
		if (current.row > 0 && cell[current.row - 1][current.col].down && !visited[current.row - 1][current.col]) { // 윗쪽으로 갈 수 있는 경우
			cell[current.row - 1][current.col].came_from = down_dir;
			addq(cell[current.row - 1][current.col]);
			visited[current.row - 1][current.col] = 1;
			ofDrawLine(linex, liney, linex, liney - 40.0f);
		}
	}
	return false;
}

