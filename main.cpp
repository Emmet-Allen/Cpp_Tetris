#include <iostream>
#include <Windows.h>
#include<chrono> //Allows for time to be expressed
#include<thread> // Allows multiple functions to execute concurrently
#include<vector>
#include <stdio.h>
#include<string>

using namespace std;

wstring tetromino[7];
int nFieldWidth = 12;
int nFieldHeight = 18; 
unsigned char* pField = nullptr; //Stored Dynamically so all map information is in one Array

int nScreenWidth = 120; // Console Screen Size X
int nScreenHeight = 60; // Console Screen Size Y


// Rotation is based on indexes of x and y of the tetrimino, where r is the rotation b/w 0 and 270 Degrees 
int Rotate(int px, int py, int r) {
	switch (r % 4) {
	case 0: return py * 4 + px; // 0 Degree Rotation
	case 1: return 12 + py - (px * 4); // 90 Degree Rotation
	case 2: return 15 - (py * 4) - px; // 180 Degree Rotation
	case 3: return 3 - py + (px * 4); // 270 Degree Rotation
	}
}

bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY) {
	for (int px = 0; px < 4; px++) {
		for (int py = 0; py < 4; py++) {

			//Get index of array of tetromino piece
			int pi = Rotate(px, py, nRotation);

			//Get index of array of tetris playing field
			int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

			if (nPosX + px >= 0 && nPosX + px < nFieldWidth) {

				if (nPosY + py >= 0 && nPosY + py < nFieldHeight) {
					
					if (tetromino[nTetromino][pi] == L'X' && pField[fi] != 0) {
						return false; //Detects Collusion: piece index is occupied and playing field index is occupied
					}
				}
			}
		}
	}

	return true; 
}

int main() {
	//Creating Tetrominos

	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");

	tetromino[1].append(L"..X.");
	tetromino[1].append(L".XX.");
	tetromino[1].append(L".X..");
	tetromino[1].append(L"....");

	tetromino[2].append(L".X..");
	tetromino[2].append(L".XX.");
	tetromino[2].append(L"..X.");
	tetromino[2].append(L"....");

	tetromino[3].append(L"..X.");
	tetromino[3].append(L"..XX.");
	tetromino[3].append(L"..X.");
	tetromino[3].append(L"....");

	tetromino[4].append(L"....");
	tetromino[4].append(L".XX.");
	tetromino[4].append(L".XX.");
	tetromino[4].append(L"....");

	tetromino[5].append(L"....");
	tetromino[5].append(L"..X.");
	tetromino[5].append(L"..X.");
	tetromino[5].append(L"..XX");

	tetromino[6].append(L"....");
	tetromino[6].append(L"..X.");
	tetromino[6].append(L"..X.");
	tetromino[6].append(L".XX.");

	pField = new unsigned char[nFieldWidth * nFieldHeight]; //Creates Dynamic Array
	for (int x = 0; x < nFieldWidth; x++) // Board Boundary
	{
		for (int y = 0; y < nFieldHeight; y++) {
			pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0; // 9 is the boarder otherwise set the array value area as '0' or 'empty space' 
		}
	}

	//Screen Buffer 
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	for (int i = 0; i < nScreenWidth * nScreenHeight; i++) {
		screen[i] = L' ';
	}
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	//Game Loops (Used to output certain things until game is finished or user exits)

	bool bGameOver = false;

	//GAME STATE ===========================================

	//Game Logic
	int nCurrentPiece = 3; //Current Piece: Is it Falling?
	int nCurrentRotation = 0; //What angle is the piece currently?
	int nCurrentX = nFieldWidth / 2; //Where is it in the field according to width. Piece will fall in middle of field
	int nCurrentY = 0; // Where is it in the field according to height 

	bool bKey[4];
	bool bRotateHold = false;

	int nSpeed = 20; //Current Speed
	int nSpeedCounter = 0; // Counts Game Ticks
	bool bForceDown = false;
	int nPieceCount = 0;
	int nScore = 0;

	vector<int> vLines;

	while (!bGameOver) {

		// GAME TIMING=======================================
		this_thread::sleep_for(50ms); //Gives movement tick every .5sec
		nSpeedCounter++;
		bForceDown = (nSpeedCounter == nSpeed);


		// INPUT=============================================
		for (int k = 0; k < 4; k++) {
			//Piece movement by calling Async Key State to movement arrows (Right\Left\Down\Z) to see if key state is bool in an array
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28\Z"[k]))) != 0;
		}

		// GAME LOGIC========================================


		//OLD LOGIC
		//if (bKey[1]) {
		//	if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) {
		//		nCurrentX = nCurrentX - 1;
		//	}
		//}


		//Right Key
			// For nested 'If' loops, we can usually use && to satisfy the logic 
		nCurrentX += (bKey[0] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0;

		//Left Key 
		nCurrentX -= (bKey[1] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0;

		//Down Key
		nCurrentY += (bKey[2] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? 1 : 0;

		//Rotation (Z) Key
		if (bKey[3]) {
			nCurrentRotation += (!bRotateHold && DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0;
			bRotateHold = true;
		}
		else {
			bRotateHold = false;
		}

		//Forcing Piece Down

		if (bForceDown) {
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) {
				nCurrentY++;
			}
			else {
				//Lock current piece in the field
				for (int px = 0; px < 4; px++) {
					for (int py = 0; py < 4; py++) {
						//If there is an empty space
						if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X') {
							//Update playing field and occupy space
							pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;
						}
					}
				}

				nPieceCount++;
				if (nPieceCount % 10 == 0) {
					if (nSpeed >= 10) {
						nSpeed--;
					}
				}
			

				//Check to see if we have any lines
				for (int py = 0; py < 4; py++) {
					if (nCurrentY + py < nFieldHeight - 1) {

						bool bLine = true;
						for (int px = 1; px < nFieldWidth - 1; px++) {
							bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;
						}

						if (bLine) {
							//Remove Line, set to =
							for (int px = 1; px < nFieldWidth - 1; px++) {
								pField[(nCurrentY + py) * nFieldWidth + px] = 8;

								//Remeber to ALWAYS CHECK FOR SCOPE!!!! 
								/*vLines.push_back(nCurrentY + py);*/
							}
							vLines.push_back(nCurrentY + py);
						}
					}
				}

				nScore += 25;
				if (!vLines.empty()) {
					nScore += (1 << vLines.size()) * 100;
				}

				//Choose next piece
				nCurrentX = nFieldWidth / 2; //New Piece will fall in middle of field
				nCurrentY = 0; // New Piece will always fall from the top 
				nCurrentRotation = 0; //No Angle of rotation b/c new piece
				nCurrentPiece = rand() % 7; //Choose Random Piece



			   // if piece does not fit
				bGameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
			}

			nSpeedCounter = 0;
		}

		//RENDER OUTPUT======================================

		//Draw Field 
		for (int x = 0; x < nFieldWidth; x++) {
			for (int y = 0; y < nFieldHeight; y++) {
			screen[(y + 2)*nScreenWidth + (x + 2)] = L" ABCDEFG=#"[pField[y * nFieldWidth + x]];

			}
		}

		//Draw Current Piece
		for (int px = 0; px < 4; px++) {
			for (int py = 0; py < 4; py++) {
				// If Current Piece is Being placed in an unoccupied space
				if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X') {
					//Draw the Piece on the Screen with a Corresponding ASCII letter starting with 'A'
					screen[(nCurrentY + py + 2) * nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65;
				}
			}
		}

		//Draw Score
		swprintf_s(&screen[2 * nScreenWidth + nFieldWidth + 6], 16, L"SCORE: %8d", nScore);

		if (!vLines.empty()) {
			//Display Frame 
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
			this_thread::sleep_for(400ms); // Delay 

			for (auto &v : vLines){
				for (int px = 1; px < nFieldWidth - 1; px++) {
					for (int py = v; py > 0; py--) {
						pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
					}
					pField[px] = 0;
				}
			}

					vLines.clear();
				
		}


			//Display Frame
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
		}

		//Game Over
		CloseHandle(hConsole);
		cout << "Game Over!! Score:" << nScore << endl;
		
		string Enter;
		cout << "Press Enter to Exit" << endl;
		getline(cin, Enter);



	return 0;
}