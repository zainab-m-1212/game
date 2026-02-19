Checkers and Othello (Reversi) — C++ / Qt

Overview

Network Board Games Platform is a complete C++/Qt project that implements two classic board games, 
Checkers and Othello (Reversi), with a client/server network system and a professional graphical user interface. The project includes authentication features such as Login and Signup. 
It is designed for academic purposes, network education, advanced programming, and software design, using a modular and extensible architecture.

Key Features

Authentication with Login and Signup including full field validation and secure user management.

Secure password hashing using SHA‑256 and safe storage of user data in a users.txt file.

Multi‑game platform supporting two independent network games, Checkers and Othello, each with complete game logic.

Modular client/server architecture.

Modern and responsive graphical user interface developed using Qt Widgets.

Project Structure

Authentication Module

loginwindow.ui, loginwindow.h, loginwindow.cpp

Login window implementation.

signupwindow.ui, signupwindow.h, signupwindow.cpp

Signup window for new user registration.

usermanager.h, usermanager.cpp

Core user management and authentication logic.

Checkers Game Module

checkersgame.h, checkersgame.cpp

Core game logic and rules for Checkers.

checkersgamewidget.h, checkersgamewidget.cpp

Graphical widget and rendering logic for the Checkers game board.

checkersclient.h, checkersclient.cpp

Network client implementation for the Checkers game.

checkersserver.h, checkersserver.cpp

Network server implementation for the Checkers game.

Othello (Reversi) Game Module

reversigame.h, reversigame.cpp

Core game logic and rules for Othello.

reversigamewidget.h, reversigamewidget.cpp

Graphical widget and rendering logic for the Othello game board.

othelloclient.h, othelloclient.cpp

Network client implementation for the Othello game.

othelloserver.h, othelloserver.cpp

Network server implementation for the Othello game.

Core Application

main.cpp

Application entry point.

mainwindow.h, mainwindow.cpp, mainwindow.ui

Main window for game selection and navigation.

Build System

Makefile

Automatically generated build file.

Qt project file (.pro)

Authentication System

UserManager

Responsible for hashing passwords using the SHA‑256 algorithm.

Validates phone number and email format during signup.

Registers new users with username uniqueness checking.

Authenticates user credentials during login.

Supports password recovery based on the initial design.

Users file format (users.txt)

Each line represents one user and fields are separated by the | character.

username | hashedPassword | name | phone | email

Signup Window

Files: signupwindow.ui, signupwindow.h, signupwindow.cpp

Features

Card‑style UI for the signup form.

Collects and validates all required fields including full name, username, phone number, email, and password.

Displays clear error messages when validation rules are violated.

Shows a success message after successful registration.

Provides a button to return to the Login window.

Login Window

Files: loginwindow.ui, loginwindow.h, loginwindow.cpp

Features

Simple interface for login using username and password.

Validates entered credentials using the UserManager class.

Redirects the user to the MainWindow after successful login.

Provides a link or button to navigate to the Signup window for new users.

Game Logic

Checkers (Draughts)

Full implementation of movement and capture rules.

Accurate turn management between players.

Detection of game‑over conditions including winner, loser, or draw.

Implementation of king promotion and special king movement abilities.

Othello (Reversi)

Standard 8x8 game board.

Algorithm for validating moves in all eight directions (horizontal, vertical, diagonal).

Automatic flipping of captured opponent pieces.

Pass functionality when a player has no valid moves and the turn switches automatically.

Final score calculation based on piece count and winner detection.

GUI Design

Developed using Qt Widgets.

Responsive layouts using QVBoxLayout, QHBoxLayout, and QGridLayout.

Display of game status including current turn and player scores.

Hint functionality to highlight valid moves for the current player.

End‑game message box displaying the result and offering options to restart or exit.

Network Architecture

Client/server communication is established using JSON message exchange.

This architecture enables real‑time multiplayer gameplay over the network.
