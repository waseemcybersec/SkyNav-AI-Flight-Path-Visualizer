#include"optimalPath.h"
#include "Booking.h"
#include"layoverMang.h"
#include"advanceRoute.h"
#include"subGraph.h"

void waiting()
{
    for (int i = 0; i < 10; i++)
    {
        Sleep(200);
        cout << "..";
    }
}

string EncryptDecrypt(string data)
{
    char key = 'K';
    string result = data;

    for (int i = 0; i < data.length(); ++i)
        result[i] = data[i] ^ key;

    return result;
}

void registration()
{
    string password, name;

    cout << endl;
    cout << " __________________________________________\n";
    cout << "|                                          |\n";
    cout << "|               Create Account             |" << endl;
    cout << "|__________________________________________|" << endl << endl;
    cout << "Enter Username: ";

    if (cin.peek() == '\n') {
        cin.ignore();
    }
    getline(cin, name);


    cout << "Enter Password: ";
    char ch;
    password = "";
    ch = _getch();

    while (ch != 13)
    {
        password.push_back(ch);
        cout << '*';
        ch = _getch();
    }

    // encrypting the password
    password = EncryptDecrypt(password);

    ofstream fout("credential.txt", ios::app);
    if (!fout)
    {
        cout << "File not found\n";
        exit(0);
    }

    else
    {
        fout << name << " " << password << "\n";
        fout.close();

        cout << endl;
        cout << " ____________________________________________\n";
        cout << "|                                            |\n";
        cout << "|           REGISTRATION SUCCESSFUL          |" << endl;
        cout << "|____________________________________________|" << endl << endl;
    }
}

void login()
{
label1:
    string username, name;
    string pass, password;
    bool login = false;

    cout << "\n| Enter Username: ";
    if (cin.peek() == '\n')
    {
        cin.ignore();
    }
    getline(cin, username);


    cout << "| Enter Password: ";
    char ch;
    pass = "";
    ch = _getch();
    while (ch != 13)
    {
        pass.push_back(ch);
        cout << '*';
        ch = _getch();
    }

    ifstream fin("credential.txt");
    if (!fin)
    {
        cout << "\nFile not found!!\n";
        exit(0);
    }
    else
    {
        while (fin >> name >> password)
        {
            // decrypting the password
            string decryptedPassword = EncryptDecrypt(password);
            if (username == name && pass == decryptedPassword)
            {
                login = true;
                break;
            }
        }
        fin.close();
    }

    if (login)
    {
        cout << "\n\nVerifying user data!\n";
        waiting();

        system("cls");
        cout << endl;
        cout << "    ________________________________________\n";
        cout << "   |                                        |\n";
        cout << "   |           LOGIN SUCCESSFUL             |" << endl;
        cout << "   |\t       Logined As " << name << "              |" << endl;
        cout << "   |________________________________________|" << endl << endl;
        Sleep(789);
    }
    else
    {
        cout << "\n\nIncorrect username or password\n";
        cout << "Please try again\n\n";
        goto label1;
    }
}

void displayMenu()
{
    cout << endl << endl;
    cout << "| 1. View All Flights                           |\n";
    cout << "| 2. Book a Flight                              |\n";
    cout << "| 3. Find optimal Route                         |\n";
    cout << "| 4. Filter Flights by Preferences              |\n";
    cout << "| 5. Layover management                         |\n";
    cout << "| 6. Advance route generation                   |\n";
    cout << "| 7. Create a Subgraph                          |\n";
    cout << "| 8. Exit                                       |\n";
    cout << "| Enter your choice: ";
}

void plane()
{
    cout << "                                                -~                               :-            " << endl;
    cout << "                                               -**:                       :=~.  ~>)=           " << endl;
    cout << "                                              :*+~+.                     :=:~=~^*~=:           " << endl;
    cout << "                                              -^~=*:                     :=::+>=:=:            " << endl;
    cout << "                                              ~^~+~+:                    :+:+>~:=^-            " << endl;
    cout << "                                              ~^~:+^:                    :=*>-=*^<*.           " << endl;
    cout << "                                              ~^~:+~>:                  .^]>*++~:::::=^>~      " << endl;
    cout << "                                            -+**~:~~*:               .+~~)*:=))^^>>>>>>*+:     " << endl;
    cout << "                                           =)++*~::+~^:            +~:....:-~:                 " << endl;
    cout << "                                            ::=+~::+~*:         ==-......-=~                   " << endl;
    cout << "                                              ~+~:::+-<:     -~~.......-=-.                    " << endl;
    cout << "                                             .+*~:::+-*:  :~=.......:~=-                       " << endl;
    cout << "                                           :==*+~-::==:^==.......:-==.                         " << endl;
    cout << "                                         .=+==^+~-::-^^:.......:~=-                            " << endl;
    cout << "                                          -*~.==~~~+-.......:-==                               " << endl;
    cout << "   __       __  __    __                      =**=.......-~><+. .                              " << endl;
    cout << "  |  \\  _  |  \\|  \\  |  \\                    ~=......:==>>^**-::-+^(+                      " << endl;
    cout << "  | $$ / \\ | $$| $$  | $$                 :=:.....:*^<>-:::::::::~^^*+..-***+:                " << endl;
    cout << "  | $$/  $\\| $$| $$  | $$              :=:......=+^^++^^**^^+~...........:~*^*=:=**+:         " << endl;
    cout << "  | $$  $$$\\ $$| $$  | $$           .~:.....:~+^^^^*+~-::::....::..:-~=+**+*+=~-::-=***=+>=   " << endl;
    cout << "  | $$ $$\\$$\\$$| $$  | $$         .>)=-...:~+~.         .+*-+=*.....::=^~=^**++***+**^>^-    " << endl;
    cout << "  | $$$$  \\$$$$| $$__/ $$        =~-~-.:~=-.            ~^~~-.       ~=--+~.                  " << endl;
    cout << "  | $$$    \\$$$ \\$$    $$       +~::~==-.                .:          ~^=-                    " << endl;
    cout << "   \\$$      \\$$  \\$$$$$$        =~~-.                                                       " << endl;
    cout << "  ______   __            __  __                                                                " << endl;
    cout << " /      \\ |  \\          |  \\|  \\                                                           " << endl;
    cout << "|  $$$$$$\\ \\$$  ______  | $$ \\$$ _______    ______    _______                               " << endl;
    cout << "| $$__| $$|  \\ /      \\ | $$|  \\|       \\  /      \\  /       \\							" << endl;
    cout << "| $$    $$| $$|  $$$$$$\\| $$| $$| $$$$$$$\\|  $$$$$$\\|  $$$$$$$								" << endl;
    cout << "| $$$$$$$$| $$| $$   \\$$| $$| $$| $$  | $$| $$    $$ \\$$    \\								" << endl;
    cout << "| $$  | $$| $$| $$      | $$| $$| $$  | $$| $$$$$$$$ _\\$$$$$$\\								" << endl;
    cout << "| $$  | $$| $$| $$      | $$| $$| $$  | $$ \\$$     \\|       $$								" << endl;
    cout << " \\$$   \\$$ \\$$ \\$$       \\$$ \\$$ \\$$   \\$$  \\$$$$$$$ \\$$$$$$$						" << endl << endl;
}

void start()
{
    plane();
label2:
    cout << "\n\n";
    cout << "| 1. REGISTRATION               |\n";
    cout << "| 2. LOGIN                      |\n";
    cout << "| Enter your choice: ";
    char ch;
    cin >> ch;

    if (ch == '1')
    {
        registration();
        goto label2;
    }

    else if (ch == '2')
        login();
}

int main()
{
    start();
    allFlights AF;
    AF.extract_data_from_file();
    AF.read_hotel_prices();

    int choice;
    do
    {
        plane();
        displayMenu();
        cin >> choice;
        cout << endl;

        switch (choice)
        {
        case 1:
            system("cls");
            AF.runAllFlights();
            AF.display_graph();
            break;

        case 2:
        {
            system("cls");
            BookFlight b(&AF);
            b.book_flight();
            break;
        }

        case 3:
        {
            system("cls");
            OptimalPath op(&AF);
            op.find_flight();
            break;
        }

        case 4:
        {
            system("cls");
            CustomFlightSearch CF(&AF);
            CF.search_preferred_flights();
            break;
        }

        case 5:
        {
            system("cls");
            string origin, destination;

            cout << "Enter the Origin City: ";
            cin >> origin;

            cout << "Enter the Destination City: ";
            cin >> destination;

            LayoverManager LM(&AF);
            LM.manage_journey(origin, destination);
            break;
        }

        case 6:
        {
            system("cls");
            AdvancedRouteGenerator ARG(&AF);
            ARG.generate_route();
            break;
        }

        case 7:
        {
            system("cls");
            string airline;
            cout << "\nEnter Airlines Name: ";
            cin >> airline;

            AirlineQuery AQ(&AF);
            AQ.create_airline_subgraph(airline);
            AF.runSubGraph(airline);
            AQ.display_subgraph();
            break;
        }

        case 8:
        {
            cout << "Thankyou For Visiting WU Airlines. Goodbye!\n";
            waiting();
            exit(0);
            break;
        }


        default:
            cout << "Invalid choice! Please try again.\n";
        }
    } while (choice != 8);

    return 0;
}