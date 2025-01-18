#include <iostream>
#include <pqxx/pqxx>
#include "Functions.h"

using namespace std;
using namespace pqxx;

int main(int argc, char* argv[])
{

    try
    {
        connection C("dbname=default user=postgres password=postgres hostaddr=127.0.0.1 port=5432");

        if (C.is_open())
        {
            cout << "Opened database successfully: " << C.dbname() << endl;
        } 
        else
        {
            cout << "Can't open database" << endl;
            return 1;
        }

        int option;      

        while (true)
        {
            showNewAccountMenu();
            cout << "Option: ";
            cin >> option;

            system("clear");

            switch (option)
            {
                case 1:
                {
                    string nume, prenume, dataNasterii, cnp, strada, numar, judet_sector, oras, tara, adresa_email, nr_telefon;
                    
                    adaugaUtilizatorNou(nume, prenume, dataNasterii, cnp, strada, numar, judet_sector, oras, tara, adresa_email, nr_telefon);
                        
                    utilizatorNou(nume, prenume, dataNasterii, cnp, strada, numar, judet_sector, oras, tara, adresa_email, nr_telefon, C);
                    break;
                }

                case 2:
                {
                    int pin;
                    bool validInput = false;
                    while (!validInput)
                    {
                        cout << "Va rugam introduceti PIN-ul: ";
                        cin >> pin;
                        if(cin.fail())
                        {
                            cout << "PIN-ul trebuie sa contina doar cifre." << endl;
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        }
                        else
                            validInput = true;
                    }
                    
                    check_pin(pin, option, C);                        
                    break;
                }
                case 3:
                    cout << "Multumim ca ati folosit ATM-ul!" << endl;
                    return 0;
                default:
                    cout << "Va rugam selectati o optiune valida!" << endl;

            }
        }
    }
    catch (const exception &e)
    {
        cerr << e.what() << endl;
        return 1;
    }
}