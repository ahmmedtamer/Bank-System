/*
	Bank System
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <iomanip>

using namespace std;

string ClientFileName = "Clients.txt";
string UserFileName = "Users.txt";

enum enMainMenuOption
{
	enListClients = 1, enAddClients = 2, enDeleteClients = 3,
	enUpdateClients = 4, enFindClients = 5, enShowTransactionsMenu = 6, enManageUsers, enLogout = 8
};

enum enTransactionsMenuOption
{
	enDeposit = 1, enWithdraw = 2, enShowTotalBalance = 3, enShowMainMenu = 4
};

enum enManageUsersMenuOption
{
	enListUsers = 1, enAddNewUser = 2, enDeleteUser = 3, enUpdateUser = 4, enFindUser = 5,
	enMainMenuShow = 6
};

enum enMainMenuPermissions
{
	enAllPermissions = -1, enShowClientListpermission = 1, enAddNewClientpermission = 2,
	enDeleteClientpermission = 4, enUpdateClientpermission = 8, enFindClientpermission = 16,
	enTransactionspermission = 32, enManageUserpermission = 64
};

struct stClient
{
	string AccountNumber = "";
	string PinCode = "";
	string Name = "";
	string Phone = "";
	double AccountBalance = 0;
	bool MarkForDelete = false;
};

struct stUser
{
	string Username = "";
	string Password = "";
	int permissions = 0;
	bool MarkForDelete = false;
};

stUser CurrentUser;

void MainMenuScreen();
void ShowTransactionsMenuScreen();
void Login();
void ShowManageUsersMenuScreen();
void ShowAccessDeniedMessage();
bool CheckAccessPermission(enMainMenuPermissions MainMenuPermissions);

short ReadMainMenuOption()
{
	short NumberOp = 0;

	cout << "Choose what do you want to do? [1 to 8]? ";
	cin >> NumberOp;

	return NumberOp;
}

short ReadTransactionsMenuOption()
{
	short NumberOp = 0;

	cout << "Choose what do you want to do? [1 to 4]? ";
	cin >> NumberOp;

	return NumberOp;
}

short ReadManageUserMenuOption()
{
	short NumberOp = 0;

	cout << "Choose what do you want to do? [1 to 6]? ";
	cin >> NumberOp;

	return NumberOp;
}

string ReadClientAccountNumber()
{
	string AccountNumber;

	cout << "\nPlease enter AccountNumber? ";
	cin >> AccountNumber;

	return AccountNumber;
}

string ReadUser()
{
	string User;

	cout << "Enter Username? ";
	cin >> User;

	return User;
}

string ReadPassword()
{
	string Password;

	cout << "Enter Password? ";
	cin >> Password;

	return Password;
}

int Readpermissions()
{
	char Answer = 'n';

	cout << "\nDo you want to give full access? y/n? ";
	cin >> Answer;

	if (toupper(Answer) == 'Y')
	{
		return -1;
	}

	else
	{
		int TotalPermissions = 0;

		cout << "\nDo you want to give access to :\n";

		cout << "\nShow Client List? y/n? ";
		cin >> Answer;

		if (toupper(Answer) == 'Y')
		{
			TotalPermissions += enMainMenuPermissions::enAllPermissions;
		}

		cout << "\nAdd New Client? y/n? ";
		cin >> Answer;

		if (toupper(Answer) == 'Y')
		{
			TotalPermissions += enMainMenuPermissions::enAddNewClientpermission;
		}

		cout << "\nDelete Client? y/n? ";
		cin >> Answer;

		if (toupper(Answer) == 'Y')
		{
			TotalPermissions += enMainMenuPermissions::enDeleteClientpermission;
		}

		cout << "\nUpdate Client? y/n? ";
		cin >> Answer;

		if (toupper(Answer) == 'Y')
		{
			TotalPermissions += enMainMenuPermissions::enUpdateClientpermission;
		}

		cout << "\nFind Client? y/n? ";
		cin >> Answer;

		if (toupper(Answer) == 'Y')
		{
			TotalPermissions += enMainMenuPermissions::enFindClientpermission;
		}

		cout << "\nTransactions? y/n? ";
		cin >> Answer;

		if (toupper(Answer) == 'Y')
		{
			TotalPermissions += enMainMenuPermissions::enTransactionspermission;
		}

		cout << "\nManage Users? y/n? ";
		cin >> Answer;

		if (toupper(Answer) == 'Y')
		{
			TotalPermissions += enMainMenuPermissions::enManageUserpermission;
		}

		return TotalPermissions;
	}
}

vector <string> SplittingString(string stLine, string Delim)
{
	vector <string> vString;

	string sWord = "";
	short Pos = 0;

	while ((Pos = stLine.find(Delim)) != stLine.npos)
	{
		sWord = stLine.substr(0, Pos);

		if (sWord != "")
		{
			vString.push_back(sWord);
		}

		stLine.erase(0, Pos + Delim.length());
	}

	if (stLine != "")
		vString.push_back(stLine);

	return vString;
}

stClient ConvertLineToRecord(string stLine, string Separator = "#//#")
{
	stClient Client;
	vector <string> vString = SplittingString(stLine, Separator);

	Client.AccountNumber = vString[0];
	Client.PinCode = vString[1];
	Client.Name = vString[2];
	Client.Phone = vString[3];
	Client.AccountBalance = stod(vString[4]);

	return Client;
}

vector <stClient> LoadClientsDataFromFile(string FileName)
{
	vector <stClient> vClients;
	fstream MyFile;

	MyFile.open(FileName, ios::in);
	if (MyFile.is_open())
	{
		string stLine = "";
		stClient Client;

		while (getline(MyFile, stLine))
		{
			Client = ConvertLineToRecord(stLine);
			vClients.push_back(Client);
		}
	}

	return vClients;
}

bool ClientExistsByAccountNumber(string AccountNumber, string FileName)
{
	fstream MyFile;
	MyFile.open(FileName, ios::in);

	if (MyFile.is_open())
	{
		string stLine;

		while (getline(MyFile, stLine))
		{
			stClient Client = ConvertLineToRecord(stLine);
			if (Client.AccountNumber == AccountNumber)
			{
				MyFile.close();
				return true;
			}
		}
		MyFile.close();
	}
	return false;
}

void ShowClientListOutputFormatted()
{
	cout << "| " << setw(15) << left << "Account Number";
	cout << "| " << setw(12) << left << "Pin Code";
	cout << "| " << setw(40) << left << "Client Name";
	cout << "| " << setw(15) << left << "Phone";
	cout << "| " << setw(15) << left << "Balance";
}

void PrintClientListScreen()
{
	vector <stClient> vClients = LoadClientsDataFromFile(ClientFileName);

	if (vClients.size() == 0)
	{
		cout << "\t\t\t\tNo Clients Available In the System! \n";
	}
	else
	{
		for (stClient& C : vClients)
		{
			cout << "| " << setw(15) << left << C.AccountNumber;
			cout << "| " << setw(12) << left << C.PinCode;
			cout << "| " << setw(40) << left << C.Name;
			cout << "| " << setw(15) << left << C.Phone;
			cout << "| " << setw(15) << left << C.AccountBalance;
			cout << endl;
		}
	}
}

stClient ReadNewClient()
{
	stClient Client;

	cout << "\nEnter Account Number? ";
	getline(cin >> ws, Client.AccountNumber);

	while (ClientExistsByAccountNumber(Client.AccountNumber, ClientFileName))
	{
		cout << "\nClient with [" << Client.AccountNumber << "] already exists, Enter another Account Number ? ";
		getline(cin >> ws, Client.AccountNumber);
	}

	cout << "Enter PinCode? ";
	getline(cin, Client.PinCode);

	cout << "Enter Name? ";
	getline(cin, Client.Name);

	cout << "Enter Phone? ";
	getline(cin, Client.Phone);

	cout << "Enter AccountBalance? ";
	cin >> Client.AccountBalance;

	return Client;
}

stClient AddNewClient()
{
	stClient Client = ReadNewClient();
	return Client;
}

string ConvertRecordToLine(stClient Client, string Separator = "#//#")
{
	string stLine = "";

	stLine += Client.AccountNumber + Separator;
	stLine += Client.PinCode + Separator;
	stLine += Client.Name + Separator;
	stLine += Client.Phone + Separator;
	stLine += to_string(Client.AccountBalance);

	return stLine;
}

void AddDataLineToFile(stClient Client, string FileName)
{
	fstream MyFile;

	MyFile.open(FileName, ios::out | ios::app);

	if (MyFile.is_open())
	{
		string stLine = ConvertRecordToLine(Client);
		MyFile << stLine << endl;

		MyFile.close();
	}
}

void AddNewClients()
{
	char Answer = 'n';

	do
	{
		cout << "Adding New Client:\n";
		stClient Client = AddNewClient();
		AddDataLineToFile(Client, ClientFileName);
		cout << "\nClient Added Successfully, do you want to add more clients? Y/N? ";
		cin >> Answer;
	} while (toupper(Answer) == 'Y');
}

bool FindClientByAccountNumber(vector <stClient> vClients, stClient& Client, string AccountNumber)
{
	for (stClient& C : vClients)
	{
		if (C.AccountNumber == AccountNumber)
		{
			Client = C;
			return true;
		}
	}
	return false;
}

void PrintClientCard(stClient Client)
{
	cout << "\nThe Following are the client details:\n";
	cout << "_____________________________________";
	cout << "\nAccount Number: " << Client.AccountNumber;
	cout << "\nPin Code: " << Client.PinCode;
	cout << "\nName: " << Client.Name;
	cout << "\nPhone: " << Client.Phone;
	cout << "\nAccount Balance: " << Client.AccountBalance;
	cout << "\n_____________________________________\n";
}

bool MarkClientForDeleteByAccountNumber(vector <stClient>& vClients, string AccountNumber)
{
	for (stClient& C : vClients)
	{
		if (C.AccountNumber == AccountNumber)
		{
			C.MarkForDelete = true;
			return true;
		}
	}
	return false;
}

void SaveClientDataToFile(string FileName, vector <stClient> vClients)
{
	fstream MyFile;

	MyFile.open(FileName, ios::out);

	if (MyFile.is_open())
	{
		for (stClient& C : vClients)
		{
			if (C.MarkForDelete == false)
			{
				string stLine = ConvertRecordToLine(C);
				MyFile << stLine << endl;
			}
		}
		MyFile.close();
	}
}

bool DeleteClientByAccountNumber()
{
	vector <stClient> vClients = LoadClientsDataFromFile(ClientFileName);
	string AccountNumber = ReadClientAccountNumber();
	stClient Client;

	char Answer = 'n';

	if (FindClientByAccountNumber(vClients, Client, AccountNumber))
	{
		PrintClientCard(Client);

		cout << "\n\nAre you sure you want delete this Client? Y/N? ";
		cin >> Answer;

		if (toupper(Answer) == 'Y')
		{
			MarkClientForDeleteByAccountNumber(vClients, AccountNumber);
			SaveClientDataToFile(ClientFileName, vClients);

			vClients = LoadClientsDataFromFile(ClientFileName);
			cout << "\n\nClient Deleted Successfully.\n";
			return true;
		}

	}

	else
	{
		cout << "Client with Account Number (" << AccountNumber << ") is Not Found!\n";
		return false;
	}
}

stClient ChangeClientRecord(string AccountNumber)
{
	stClient Client;

	Client.AccountNumber = AccountNumber;

	cout << "\nEnter PinCode? ";
	getline(cin >> ws, Client.PinCode);

	cout << "Enter Name? ";
	getline(cin, Client.Name);

	cout << "Enter Phone? ";
	getline(cin, Client.Phone);

	cout << "Account Balance? ";
	cin >> Client.AccountBalance;

	return Client;
}

bool UpdateClientByAccountNumber()
{
	vector <stClient> vClients = LoadClientsDataFromFile(ClientFileName);
	string AccountNumber = ReadClientAccountNumber();
	stClient Client;
	char Answer = 'n';

	if (FindClientByAccountNumber(vClients, Client, AccountNumber))
	{
		PrintClientCard(Client);

		cout << "\n\nAre you sure you want update this client? Y/N? ";
		cin >> Answer;

		if (toupper(Answer) == 'Y')
		{
			stClient Client = ChangeClientRecord(AccountNumber);
			for (stClient& C : vClients)
			{
				if (C.AccountNumber == AccountNumber)
				{
					C = Client;
					break;
				}
			}

			SaveClientDataToFile(ClientFileName, vClients);
			cout << "\n\nClient Update Successfully\n";

			return true;
		}
	}
	else
	{
		cout << "Client with Account Number (" << AccountNumber << ") is Not Found!\n";
		return false;
	}
}

bool FindClient()
{
	vector <stClient> vClients = LoadClientsDataFromFile(ClientFileName);
	string AccountNumber = ReadClientAccountNumber();
	stClient Client;

	if (FindClientByAccountNumber(vClients, Client, AccountNumber))
	{
		PrintClientCard(Client);
		return true;
	}
	else
	{
		cout << "Client with Account Number [" << AccountNumber << "] is Not Found!\n";
		return false;
	}
}

bool DepositBalanceCalculation(string AccountNumber, double Amount, vector <stClient> vClients)
{
	double NewBalance = 0;
	char Answer = 'n';

	cout << "\n\nAre you sure you want perfrom this Transactions? y/n? ";
	cin >> Answer;

	if (toupper(Answer) == 'Y')
	{
		for (stClient& C : vClients)
		{
			if (C.AccountNumber == AccountNumber)
			{
				C.AccountBalance += Amount;
				NewBalance = C.AccountBalance;
				break;
			}
		}

		SaveClientDataToFile(ClientFileName, vClients);
		cout << "\n\nDone Successfully. New balance is: " << NewBalance << endl;

		return true;
	}


	return false;
}

void DepositBalanceToClientByAccountNumber()
{
	string AccountNumber = ReadClientAccountNumber();
	vector <stClient> vClients = LoadClientsDataFromFile(ClientFileName);
	stClient Client;

	while (!FindClientByAccountNumber(vClients, Client, AccountNumber))
	{
		cout << "\nClient with [" << AccountNumber << "] does not exist.\n";
		AccountNumber = ReadClientAccountNumber();
	}

	PrintClientCard(Client);


	double DepositAmount = 0;

	cout << "\nPlease enter deposit amount? ";
	cin >> DepositAmount;

	DepositBalanceCalculation(AccountNumber, DepositAmount, vClients);
}

void ShowBalanceClientListFormatted()
{
	cout << "| " << setw(15) << left << "Account Number";
	cout << "| " << setw(40) << left << "Client Name";
	cout << "| " << setw(30) << left << "Balance";
}

void ShowBalanceClientListOutput(vector <stClient> vClients, double& TotalBalance)
{
	if (vClients.size() == 0)
	{
		cout << "\t\t\t\t\tNo Clients Available In the system!";
	}

	else
	{
		for (stClient& C : vClients)
		{
			cout << "| " << setw(15) << left << C.AccountNumber;
			cout << "| " << setw(40) << left << C.Name;
			cout << "| " << setw(30) << left << C.AccountBalance;
			TotalBalance += C.AccountBalance;
			cout << endl;
		}
	}
}

void WithdrawBalanceByAccountNumber()
{
	string AccountNumber = ReadClientAccountNumber();
	vector <stClient> vClients = LoadClientsDataFromFile(ClientFileName);
	stClient Client;

	while (!FindClientByAccountNumber(vClients, Client, AccountNumber))
	{
		cout << "\nClient with [" << AccountNumber << "] does not exist.\n";
		AccountNumber = ReadClientAccountNumber();
	}

	PrintClientCard(Client);

	double Amount = 0;
	cout << "\nPlease enter withdraw amount? ";
	cin >> Amount;

	while (Amount > Client.AccountBalance)
	{
		cout << "Amount Exceeds the balance, you can withdraw up to : " << Client.AccountBalance << endl;
		cout << "Please enter another amount? ";
		cin >> Amount;
	}

	DepositBalanceCalculation(AccountNumber, Amount * -1, vClients);
}

stUser ConvertLineToRecordUser(string stLine)
{
	stUser User;
	vector <string> sVector = SplittingString(stLine, "#//#");

	User.Username = sVector[0];
	User.Password = sVector[1];
	User.permissions = stoi(sVector[2]);

	return User;
}

vector <stUser> LoadUsersDataFromFile(string FileName)
{
	fstream Myfile;
	vector <stUser> vUsers;

	Myfile.open(FileName, ios::in);
	if (Myfile.is_open())
	{
		string stLine;
		while (getline(Myfile, stLine))
		{
			stUser User = ConvertLineToRecordUser(stLine);
			vUsers.push_back(User);
		}
		Myfile.close();
	}

	return vUsers;
}

bool UserExitsByUsername(string Username, string FileName)
{
	fstream MyFile;
	MyFile.open(FileName, ios::in);

	if (MyFile.is_open())
	{
		string stLine;
		while (getline(MyFile, stLine))
		{
			stUser User = ConvertLineToRecordUser(stLine);
			if (User.Username == Username)
				return true;
		}
		MyFile.close();
	}
	return false;
}

stUser ReadNewUser()
{
	stUser User;

	cout << "Enter Username? ";
	getline(cin >> ws, User.Username);

	while (UserExitsByUsername(User.Username, UserFileName))
	{
		cout << "Client with [" << User.Username << "] already exists, Enter another User Name ? \n";
		getline(cin >> ws, User.Username);
	}

	cout << "Enter Password? ";
	getline(cin, User.Password);

	User.permissions = Readpermissions();

	return User;
}

stUser AddNewUser()
{
	stUser User = ReadNewUser();
	return User;
}

string ConvertRecordUserToLine(stUser User, string Delim = "#//#")
{
	string stLine = "";

	stLine += User.Username + Delim;
	stLine += User.Password + Delim;
	stLine += to_string(User.permissions);

	return stLine;
}

void AddDataUserToFile(stUser User, string FileName)
{
	fstream MyFile;

	MyFile.open(FileName, ios::out | ios::app);
	if (MyFile.is_open())
	{
		string stLine = ConvertRecordUserToLine(User);
		MyFile << stLine << endl;

		MyFile.close();
	}

}

void AddingNewUsers()
{
	char ReadMore = 'Y';

	do
	{
		cout << "Adding New User:\n\n";
		stUser User = AddNewUser();
		AddDataUserToFile(User, UserFileName);
		cout << "\nUser Added Successfully, do you want to add more Users? Y/N? ";
		cin >> ReadMore;

	} while (toupper(ReadMore) == 'Y');


}

void ShowListUserScreenOutput(vector <stUser> vUsers)
{
	if (vUsers.size() == 0)
	{
		cout << "\t\t\tNo Users Available In the system!";
	}

	else
	{
		for (stUser& User : vUsers)
		{
			cout << "| " << setw(15) << left << User.Username;
			cout << "| " << setw(15) << left << User.Password;
			cout << "| " << setw(15) << left << User.permissions;
			cout << endl;
		}
	}
}

void ShowListUserScreenFormated()
{
	cout << "| " << setw(15) << left << "Username";
	cout << "| " << setw(15) << left << "Password";
	cout << "| " << setw(15) << left << "permission";
	cout << endl;
}

bool FindUserByUsername(vector <stUser> vUsers, string Username, stUser& User)
{
	for (stUser& Users : vUsers)
	{
		if (Users.Username == Username)
		{
			User = Users;
			return true;
		}
	}

	return false;
}

void PrintUserCard(stUser User)
{
	cout << "\nThe following are the User details:\n";
	cout << "_____________________________________\n";
	cout << "Username   : " << User.Username << endl;
	cout << "Password   : " << User.Password << endl;
	cout << "permission : " << User.permissions << endl;
	cout << "_____________________________________\n";
}

bool MarkUserForDeleteByUsername(vector <stUser>& vUsers, string Username)
{
	for (stUser& User : vUsers)
	{
		if (User.Username == Username)
		{
			User.MarkForDelete = true;
			return true;
		}
	}

	return false;
}

void SaveUserDataToFile(string Filename, vector <stUser> vUsers)
{
	fstream MyFile;
	MyFile.open(Filename, ios::out);

	if (MyFile.is_open())
	{
		for (stUser& User : vUsers)
		{
			if (User.MarkForDelete == false)
			{
				string stLine = ConvertRecordUserToLine(User);
				MyFile << stLine << endl;
			}
		}

		MyFile.close();
	}
}

bool DeleteUserByUsername()
{
	vector <stUser> vUsers = LoadUsersDataFromFile(UserFileName);
	string Username = ReadUser();
	stUser User;

	if (Username == "Admin")
	{
		cout << "\n\nYou cannot Delete this user.\n";
		return false;
	}

	if (FindUserByUsername(vUsers, Username, User))
	{
		PrintUserCard(User);
		char Answer;
		cout << "\n\nAre you sure you want delete this user? y/n? ";
		cin >> Answer;

		if (toupper(Answer) == 'Y')
		{
			MarkUserForDeleteByUsername(vUsers, Username);
			SaveUserDataToFile(UserFileName, vUsers);
			cout << "\n\nUser Deleted Successfully.\n";
			return true;
		}
	}
	else
	{
		cout << "User with Username (" << Username << ") is Not Found!" << endl;
		return false;
	}
}

bool UpdateUserByUsername()
{
	string Username = ReadUser();
	vector <stUser> vUsers = LoadUsersDataFromFile(UserFileName);
	stUser User;

	if (FindUserByUsername(vUsers, Username, User))
	{
		PrintUserCard(User);

		char Answer;
		cout << "\n\nAre you sure you want update this user? y/n? ";
		cin >> Answer;

		if (toupper(Answer) == 'Y')
		{
			cout << "\n\n";
			string Password = ReadPassword();
			int permission = Readpermissions();

			for (stUser& User : vUsers)
			{
				if (User.Username == Username)
				{
					User.Password = Password;
					User.permissions = permission;
					break;
				}
			}

			SaveUserDataToFile(UserFileName, vUsers);
			cout << "\n\nUser updated Successfully.\n";
			return true;
		}


	}

	else
	{
		cout << "User with Username (" << Username << ") is Not Found!" << endl;
		return false;
	}
}

bool ShowFindUsersScreen()
{
	cout << "\n_____________________________________\n";
	cout << "\tFind User Screen";
	cout << "\n_____________________________________\n\n";

	string Username = ReadUser();
	vector <stUser> vUsers = LoadUsersDataFromFile(UserFileName);
	stUser User;

	if (FindUserByUsername(vUsers, Username, User))
	{
		PrintUserCard(User);
		return true;
	}

	else
	{
		cout << "User with Username (" << Username << ") is Not Found!" << endl;
		return false;
	}
}


void ShowUpdateUserScreen()
{
	cout << "\n_____________________________________\n";
	cout << "\tUpdate Users Screen";
	cout << "\n_____________________________________\n";
	UpdateUserByUsername();
}

void ShowDeleteUserScreen()
{
	cout << "\n_____________________________________\n";
	cout << "\tDelete Users Screen";
	cout << "\n_____________________________________\n";
	DeleteUserByUsername();
}

void ShowAddUserScreen()
{
	cout << "\n_____________________________________\n";
	cout << "\tAdd New User Screen";
	cout << "\n_____________________________________\n";
	AddingNewUsers();
}

void ShowListUsersScreen()
{
	vector <stUser> vUsers = LoadUsersDataFromFile(UserFileName);
	cout << "\t\t\t\t\tClient List (" << vUsers.size() << ") User(s).\n";
	cout << "________________________________________________________________________________________________________________________\n\n";
	ShowListUserScreenFormated();
	cout << "________________________________________________________________________________________________________________________\n\n";
	ShowListUserScreenOutput(vUsers);
	cout << "________________________________________________________________________________________________________________________\n\n";

}

void GoBackToManageUsersMenu()
{
	cout << "\n\nPress any key to go back to Manage Users Menu\n";
	system("pause > 0");
	ShowManageUsersMenuScreen();
}

void PerformManageUsersMenuOption(enManageUsersMenuOption ManageUsersMenuOption)
{
	switch (ManageUsersMenuOption)
	{
	case enManageUsersMenuOption::enListUsers:

		system("cls");
		ShowListUsersScreen();
		GoBackToManageUsersMenu();
		break;

	case enManageUsersMenuOption::enAddNewUser:

		system("cls");
		ShowAddUserScreen();
		GoBackToManageUsersMenu();
		break;

	case enManageUsersMenuOption::enDeleteUser:

		system("cls");
		ShowDeleteUserScreen();
		GoBackToManageUsersMenu();
		break;

	case enManageUsersMenuOption::enUpdateUser:

		system("cls");
		ShowUpdateUserScreen();
		GoBackToManageUsersMenu();
		break;

	case enManageUsersMenuOption::enFindUser:

		system("cls");
		ShowFindUsersScreen();
		GoBackToManageUsersMenu();
		break;

	case enManageUsersMenuOption::enMainMenuShow:

		system("cls");
		MainMenuScreen();
		break;
	}
}

void ShowAccessDeniedMessage()
{
	system("cls");
	cout << "\n_____________________________________\n";
	cout << "Access Denied,\n";
	cout << "You don't Have permission To Do this,\n";
	cout << "Please Contact Your admin\n";
	cout << "_____________________________________\n";
}

bool CheckAccessPermission(enMainMenuPermissions MainMenuPermissions)
{
	if (CurrentUser.permissions == enMainMenuPermissions::enAllPermissions)
		return true;
	if ((CurrentUser.permissions & MainMenuPermissions) == MainMenuPermissions)
		return true;
	else
		return false;
}

void ShowManageUsersMenuScreen()
{
	if (!CheckAccessPermission(enMainMenuPermissions::enManageUserpermission))
	{
		ShowAccessDeniedMessage();
		return;
	}

	system("cls");
	cout << "============================================\n";
	cout << "\t\tManage Users Menu Screen\n";
	cout << "============================================\n";
	cout << "\t[1] List Users.\n";
	cout << "\t[2] Add New User.\n";
	cout << "\t[3] Delete User.\n";
	cout << "\t[4] Update User.\n";
	cout << "\t[5] Find User.\n";
	cout << "\t[6] Main Menu.\n";
	cout << "============================================\n";
	PerformManageUsersMenuOption((enManageUsersMenuOption)ReadManageUserMenuOption());
}

void GoBackToMainMenu()
{
	cout << "\n\nPress any key to go back to Main Menu...";
	system("pause > 0");
	MainMenuScreen();
}

void GoBackToTransactionsMenu()
{
	cout << "\n\nPress any key to go back to Transactions Menu...";
	system("pause > 0");
	ShowTransactionsMenuScreen();
}

void ShowTotalBalanceScreen()
{
	vector <stClient> vClients = LoadClientsDataFromFile(ClientFileName);
	double TotalBalance = 0;
	cout << "\t\t\t\t\tClient List (" << vClients.size() << ") Client(s).\n";
	cout << "________________________________________________________________________________________________________________________\n\n";
	ShowBalanceClientListFormatted();
	cout << "\n________________________________________________________________________________________________________________________\n\n";
	ShowBalanceClientListOutput(vClients, TotalBalance);
	cout << "\n________________________________________________________________________________________________________________________\n\n";
	cout << "\t\t\t\t\t\tTotal Balance = " << TotalBalance << endl;
}

void ShowWithdrawScreen()
{
	cout << "\n_____________________________________\n";
	cout << "\tWithdraw Screen";
	cout << "\n_____________________________________\n";
	WithdrawBalanceByAccountNumber();
}

void ShowDepositScreen()
{
	cout << "\n_____________________________________\n";
	cout << "\tDeposit Screen";
	cout << "\n_____________________________________\n";
	DepositBalanceToClientByAccountNumber();
}

void PerformTransactionsMenuOption(enTransactionsMenuOption TransactionsMenuOption)
{
	switch (TransactionsMenuOption)
	{
	case enTransactionsMenuOption::enDeposit:

		system("cls");
		ShowDepositScreen();
		GoBackToTransactionsMenu();
		break;

	case enTransactionsMenuOption::enWithdraw:

		system("cls");
		ShowWithdrawScreen();
		GoBackToTransactionsMenu();
		break;

	case enTransactionsMenuOption::enShowTotalBalance:

		system("cls");
		ShowTotalBalanceScreen();
		GoBackToTransactionsMenu();
		break;

	case enTransactionsMenuOption::enShowMainMenu:

		MainMenuScreen();
		break;

	default:

		system("cls");
		cout << "Error! Not Correct Number Enter Valid Number...\n";
		GoBackToTransactionsMenu();
		break;

	}
}

void ShowTransactionsMenuScreen()
{
	if (!CheckAccessPermission(enMainMenuPermissions::enTransactionspermission))
	{
		ShowAccessDeniedMessage();
		return;
	}

	system("cls");
	cout << "============================================\n";
	cout << "\t\tTransactions Menu Screen\n";
	cout << "============================================\n";
	cout << "\t[1] Deposit.\n";
	cout << "\t[2] Withdraw.\n";
	cout << "\t[3] Total Balances.\n";
	cout << "\t[4] Main Menu.\n";
	cout << "============================================\n";
	PerformTransactionsMenuOption((enTransactionsMenuOption)ReadTransactionsMenuOption());
}

void ShowClientListScreen()
{
	if (!CheckAccessPermission(enMainMenuPermissions::enShowClientListpermission))
	{
		ShowAccessDeniedMessage();
		return;
	}

	vector <stClient> vClients = LoadClientsDataFromFile(ClientFileName);
	cout << "\t\t\t\t\tClient List (" << vClients.size() << ") Client(s).\n";
	cout << "________________________________________________________________________________________________________________________\n\n";
	ShowClientListOutputFormatted();
	cout << "\n\n________________________________________________________________________________________________________________________\n\n";
	PrintClientListScreen();
	cout << "\n________________________________________________________________________________________________________________________\n";
}

void ShowAddClientsScreen()
{
	if (!CheckAccessPermission(enMainMenuPermissions::enAddNewClientpermission))
	{
		ShowAccessDeniedMessage();
		return;
	}

	cout << "\n_____________________________________\n";
	cout << "\tAdd New Clients Screen";
	cout << "\n_____________________________________\n";
	AddNewClients();
}

void ShowDeleteClientsScreen()
{
	if (!CheckAccessPermission(enMainMenuPermissions::enDeleteClientpermission))
	{
		ShowAccessDeniedMessage();
		return;
	}

	cout << "\n_____________________________________\n";
	cout << "\tDelete Clients Screen";
	cout << "\n_____________________________________\n";
	DeleteClientByAccountNumber();
}

void ShowUpdateClientsScreen()
{
	if (!CheckAccessPermission(enMainMenuPermissions::enUpdateClientpermission))
	{
		ShowAccessDeniedMessage();
		return;
	}

	cout << "\n_____________________________________\n";
	cout << "\tUpdate Client Info Screen";
	cout << "\n_____________________________________\n";
	UpdateClientByAccountNumber();
}

void ShowFindClientsScreen()
{
	if (!CheckAccessPermission(enMainMenuPermissions::enFindClientpermission))
	{
		ShowAccessDeniedMessage();
		return;
	}

	cout << "\n_____________________________________\n";
	cout << "\tFind Client Screen";
	cout << "\n_____________________________________\n";
	FindClient();
}

void ShowExitScreen()
{
	cout << "\n_____________________________________\n";
	cout << "\tProgram Ends :-)";
	cout << "\n_____________________________________\n";
}

void PerformMainMenuOption(enMainMenuOption MainMenuOption)
{

	switch (MainMenuOption)
	{
	case enMainMenuOption::enListClients:

		system("cls");
		ShowClientListScreen();
		GoBackToMainMenu();
		break;

	case enMainMenuOption::enAddClients:

		system("cls");
		ShowAddClientsScreen();
		GoBackToMainMenu();
		break;

	case enMainMenuOption::enDeleteClients:

		system("cls");
		ShowDeleteClientsScreen();
		GoBackToMainMenu();
		break;

	case enMainMenuOption::enUpdateClients:

		system("cls");
		ShowUpdateClientsScreen();
		ShowAccessDeniedMessage();
		GoBackToMainMenu();
		break;

	case enMainMenuOption::enFindClients:

		system("cls");
		ShowFindClientsScreen();
		GoBackToMainMenu();
		break;

	case enMainMenuOption::enShowTransactionsMenu:

		system("cls");
		ShowTransactionsMenuScreen();
		ShowAccessDeniedMessage();
		GoBackToMainMenu();
		break;

	case enMainMenuOption::enManageUsers:

		system("cls");
		ShowManageUsersMenuScreen();
		GoBackToMainMenu();
		break;

	case enMainMenuOption::enLogout:

		system("cls");
		Login();
		break;

	default:

		system("cls");
		cout << "Error! Not Correct Number Enter Valid Number...\n";
		GoBackToMainMenu();
		break;

	}
}

void MainMenuScreen()
{
	system("cls");
	cout << "============================================\n";
	cout << "\t\tMain Menu Screen\n";
	cout << "============================================\n";
	cout << "\t[1] Show Client List.\n";
	cout << "\t[2] Add New Client.\n";
	cout << "\t[3] Delete Client.\n";
	cout << "\t[4] Update Client.\n";
	cout << "\t[5] Find Client.\n";
	cout << "\t[6] Transactions.\n";
	cout << "\t[7] Manage Users.\n";
	cout << "\t[8] Logout.\n";
	cout << "============================================\n";
	PerformMainMenuOption((enMainMenuOption)ReadMainMenuOption());
}

void ShowLoginScreen()
{
	cout << "\n_____________________________________\n";
	cout << "\tLogin Screen";
	cout << "\n_____________________________________\n";
}

bool FindUserByUsernameAndPassword(string Username, string Password, stUser& User)
{
	vector <stUser> vUsers = LoadUsersDataFromFile(UserFileName);

	for (stUser& U : vUsers)
	{
		if (U.Username == Username && U.Password == Password)
		{
			User = U;
			return true;
		}
	}

	return false;
}

bool CheckLoginUser()
{

	bool LoginFaild = false;
	string Username, Password;

	do
	{
		system("cls");
		ShowLoginScreen();

		if (LoginFaild)
		{
			cout << "Invlaid Username/Password!\n";
		}

		Username = ReadUser();
		Password = ReadPassword();

		LoginFaild = !FindUserByUsernameAndPassword(Username, Password, CurrentUser);

	} while (LoginFaild);

	return true;
}

void Login()
{

	if (CheckLoginUser())
	{
		MainMenuScreen();
	}

}

int main()
{

	Login();
	system("puase > 0");
	return 0;
}