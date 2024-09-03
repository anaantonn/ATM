#include <iostream>
#include <pqxx/pqxx> 

using namespace std;
using namespace pqxx;

void showNewAccountMenu();
void showBalance(int pin, connection &C);
void depositMoney(int pin, double amount, connection &C);
void withdrawMoney(int pin, double amount, connection &C);
void transferMoney(int senderPin, int recipientAcnumber, double amount, connection &C);
void showMenu();
void existingAccount(int option);
void utilizatorNou(const string& nume, const string& prenume, const string& dataNasterii, const string& cnp, const string& strada, const string& numar, 
                    const string& judet_sector, const string& oras, const string& tara, const string& adresa_email, const string& nr_telefon, connection &C);
void adaugaUtilizatorNou(string& nume, string& prenume, string& dataNasterii, string& cnp, string& strada, string& numar, 
                            string& judet_sector, string& oras, string& tara, string& adresa_email, string& nr_telefon);