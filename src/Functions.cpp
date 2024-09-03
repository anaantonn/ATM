#include <iostream>
#include <pqxx/pqxx> 

using namespace std;
using namespace pqxx;

void showNewAccountMenu()
{
    cout << "**********MENU**********" << endl;
    cout << "1. Creati un cont nou" << endl;
    cout << "2. Utilizator existent" << endl;
    cout << "3. Iesire" << endl;
    cout << "************************" << endl;
}

void showBalance(int pin, connection &C)
{
    try
    {
        bool success = true;
        bool failure = false;

        nontransaction N(C);
        string sql = "SELECT * FROM SOLD JOIN UTILIZATORI ON UTILIZATORID = ID WHERE PIN = " + to_string(pin);
        result R(N.exec(sql));
        if (!R.empty())
        {
            cout << "Soldul este: " << R[0][2].as<double>() << " RON" << endl;

            int utilizatorid = R[0][0].as<int>();

            N.abort();

            work W(C);
            string sql_logs = "INSERT INTO LOGS (UTILIZATORID, DATA_ORA, SELECTIA, CONT_DESTINATAR, REUSITA) VALUES (" + W.quote(utilizatorid) +
                              + ", NOW(), " + to_string(1) + ", " + to_string(0) + ", " 
                              + (success ? "TRUE" : "FALSE") + ");";
            W.exec(sql_logs);
            W.commit();
        }
        else
        {
            cout << "Utilizator inexistent!" << endl;

            nontransaction N(C);
            string sql = "SELECT * FROM SOLD JOIN UTILIZATORI ON UTILIZATORID = ID WHERE PIN = " + to_string(pin);
            result R(N.exec(sql));
            
            int utilizatorid = R[0][0].as<int>();

            N.abort();

            work W(C);
            string sql_logs = "INSERT INTO LOGS (UTILIZATORID, DATA_ORA, SELECTIA, CONT_DESTINATAR, REUSITA) VALUES (" + W.quote(utilizatorid) +
                              + ", NOW(), " + to_string(1) + ", " + to_string(0) + ", " 
                              + (failure ? "TRUE" : "FALSE") + ");";
            W.exec(sql_logs);
            W.commit();
        }
    }

    catch (const sql_error &e)
    {
        cerr << "SQL error: " << e.what() << endl;
    }

    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << endl;
    }
}

void depositMoney(int pin, double amount, connection &C)
{
    try
    {
        nontransaction N(C);
        string sql_id = "SELECT * FROM SOLD JOIN UTILIZATORI ON UTILIZATORID = ID WHERE PIN = " + to_string(pin);
        result R(N.exec(sql_id));

        int utilizatorid = R[0][0].as<int>();

        N.abort();

        work W(C);

        bool success = true;
        bool failure = false;

        string sql = "UPDATE SOLD SET SOLD_CONT = SOLD_CONT + " + to_string(amount) + " FROM UTILIZATORI WHERE UTILIZATORID = ID AND PIN = " + to_string(pin);
        W.exec(sql);
        cout << "Depozit efectuat cu succes!" << endl;
        
        string sql_logs = "INSERT INTO LOGS (UTILIZATORID, DATA_ORA, SELECTIA, CONT_DESTINATAR, REUSITA) VALUES (" + W.quote(utilizatorid) +
                          + ", NOW(), " + to_string(2) + ", " + to_string(0) + ", " 
                          + (success ? "TRUE" : "FALSE") + ");";
        W.exec(sql_logs);
        W.commit();

    }

    catch (const sql_error &e)
    {
        cerr << "SQL error: " << e.what() << endl;
    }

    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << endl;
    }
}

void withdrawMoney(int pin, double amount, connection &C)
{
    try
    {
        work W(C);

        bool success = true;
        bool failure = false;

        string sql = "SELECT * FROM SOLD JOIN UTILIZATORI ON UTILIZATORID = ID WHERE PIN = " + to_string(pin) + ";";
        result R(W.exec(sql));
            
            int utilizatorid = R[0][0].as<int>();

        if (!R.empty() && R[0][2].as<double>() >= amount)
        {
            sql = "UPDATE SOLD SET SOLD_CONT = SOLD_CONT - " + to_string(amount) + " FROM UTILIZATORI WHERE UTILIZATORID = ID AND PIN = " + to_string(pin);
            W.exec(sql);
            string sql_logs = "INSERT INTO LOGS (UTILIZATORID, DATA_ORA, SELECTIA, CONT_DESTINATAR, REUSITA) VALUES (" + W.quote(utilizatorid) +
                              + ", NOW(), " + to_string(3) + ", " + to_string(0) + ", " 
                              + (success ? "TRUE" : "FALSE") + ");";
            W.exec(sql_logs);
            W.commit();

            cout << "Bani scosi cu succes!" << endl;
        }
        else
        {
            nontransaction N(C);
            string sql = "SELECT * FROM SOLD JOIN UTILIZATORI ON UTILIZATORID = ID WHERE PIN = " + to_string(pin);
            result R(N.exec(sql));
            
            int utilizatorid = R[0][0].as<int>();

            N.abort();

            work W(C);
            string sql_logs = "INSERT INTO LOGS (UTILIZATORID, DATA_ORA, SELECTIA, CONT_DESTINATAR, REUSITA) VALUES (" + W.quote(utilizatorid) +
                              + ", NOW(), " + to_string(3) + ", " + to_string(0) + ", " 
                              + (failure ? "TRUE" : "FALSE") + ");";
            W.exec(sql_logs);
            W.commit();

            cout << "Sold insuficient!" << endl;            
        }
    }

    catch (const sql_error &e)
    {
        cerr << "SQL error: " << e.what() << endl;
    }
    
    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << endl;
    }
}

void transferMoney(int senderPin, const string& recipientAcnumber, double amount, connection &C)
{
    try
    {
        work W(C);

        bool success = true;
        bool failure = false;

        string sql = "SELECT SOLD_CONT FROM SOLD WHERE NR_CONT = '" + recipientAcnumber + "';";
        result R_recipient(W.exec(sql));
        if (R_recipient.empty())
        {            
            int utilizatorid = R_recipient[0][0].as<int>();

            string sql_logs = "INSERT INTO LOGS (UTILIZATORID, DATA_ORA, SELECTIA, CONT_DESTINATAR, REUSITA) VALUES (" + W.quote(utilizatorid) +
                              + ", NOW(), " + to_string(4) + ", " + to_string(0) + ", " 
                              + (failure ? "TRUE" : "FALSE") + ");";
            W.exec(sql_logs);
            W.commit();

            cout << "Destinatarul nu a fost gasit!" << endl;
            return;
        }

        sql = "SELECT * FROM SOLD JOIN UTILIZATORI ON UTILIZATORID = ID WHERE PIN = " + to_string(senderPin) + ";";
        result R_sender(W.exec(sql));
        if (!R_sender.empty() && R_sender[0][2].as<double>() >= amount)
        {
            int utilizatorid = R_sender[0][0].as<int>();

            sql = "UPDATE SOLD SET SOLD_CONT = SOLD_CONT - " + to_string(amount) + " FROM UTILIZATORI WHERE UTILIZATORID = ID AND PIN = " + to_string(senderPin);
            W.exec(sql);
            sql = "UPDATE SOLD SET SOLD_CONT = SOLD_CONT + " + to_string(amount) + " WHERE NR_CONT = '" + recipientAcnumber + "';";
            W.exec(sql);

            string sql_logs = "INSERT INTO LOGS (UTILIZATORID, DATA_ORA, SELECTIA, CONT_DESTINATAR, REUSITA) VALUES (" + W.quote(utilizatorid) +
                              + ", NOW(), " + to_string(4) + ", '" + recipientAcnumber + "', " 
                              + (success ? "TRUE" : "FALSE") + ");";
            W.exec(sql_logs);

            W.commit();
            cout << "Transfer efectuat cu succes!" << endl;
        }
        else
        {
            nontransaction N(C);
            string sql = "SELECT * FROM SOLD JOIN UTILIZATORI ON UTILIZATORID = ID WHERE PIN = " + to_string(senderPin);
            result R(N.exec(sql));
            
            int utilizatorid = R[0][0].as<int>();

            N.abort();

            work W(C);
            string sql_logs = "INSERT INTO LOGS (UTILIZATORID, DATA_ORA, SELECTIA, CONT_DESTINATAR, REUSITA) VALUES (" + W.quote(utilizatorid) +
                              + ", NOW(), " + to_string(4) + ", " + to_string(0) + ", " 
                              + (failure ? "TRUE" : "FALSE") + ");";
            W.exec(sql_logs);
            W.commit();

            cout << "Sold insuficient!" << endl;
        }
    }

    catch (const sql_error &e)
    {
        cerr << "SQL error: " << e.what() << endl;
    }

    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << endl;
    }
}

void showMenu()
{
    cout << "**********MENU**********" << endl;
    cout << "1. Verifica sold" << endl;
    cout << "2. Depune bani" << endl;
    cout << "3. Scoate bani" << endl;
    cout << "4. Transfera bani" << endl;
    cout << "5. Iesire" << endl;
    cout << "************************" << endl;
}

void existingAccount()
{
    int pin;
    connection C("dbname=default user=postgres password=postgres hostaddr=127.0.0.1 port=5432");
    int option;
    
    cout << "Please enter your PIN: ";
    cin >> pin;

    while (true)
    {
        showMenu();
        cout << "Option: ";
        cin >> option;
        system("clear");
    
        switch (option)
        {
            case 1:
                showBalance(pin, C);
                break;
            case 2:
            {
                double depositAmount;
                cout << "Suma pe care doriti sa o depozitati: ";
                cin >> depositAmount;
                depositMoney(pin, depositAmount, C);
                break;
            }
                
            case 3:
            {
                double withdrawAmount;
                cout << "Suma pe care doriti sa o scoateti: ";
                cin >> withdrawAmount;
                withdrawMoney(pin, withdrawAmount, C);
                break;
            }
                
            case 4:
            {
                string acnumber;
                double transferAmount;
                cout << "Cont detinatar: ";
                cin >> acnumber;
                cout << "Suma pe care doriti sa o transferati: ";
                cin >> transferAmount;
                transferMoney(pin, acnumber, transferAmount, C);
                break;
            }
                
            case 5:
                cout << "Revenire la meniul anterior." << endl;
                return;
            default:
                cout << "Va rugam selectati o optiune valida!" << endl;
        }
    }
}

void utilizatorNou(const string& nume, const string& prenume, const string& dataNasterii, const string& cnp, const string& strada, const string& numar, 
                    const string& judet_sector, const string& oras, const string& tara, const string& adresa_email, const string& nr_telefon, connection &C)
{
    try
    {
        work W(C);

        bool success = true;
        bool failure = false;

        string sql_utilizatori = "INSERT INTO UTILIZATORI (NUME, PRENUME, DATA_NASTERII, CNP) VALUES (" +
                         W.quote(nume) + ", " + W.quote(prenume) + ", " + W.quote(dataNasterii) + ", " +
                         W.quote(cnp) + ") RETURNING id;";
        result r = W.exec(sql_utilizatori);

        if (r.size() == 0)
        {
            cerr << "Failed to insert user." << endl;
            return;
        }

        int utilizatorid = r[0][0].as<int>();

        string sql_adrese = "INSERT INTO ADRESE (UTILIZATORID, STRADA, NUMAR, JUDET_SECTOR, ORAS, TARA) VALUES (" 
                         + W.quote(utilizatorid) + ", " + W.quote(strada) + ", " + W.quote(numar) + ", " +
                         W.quote(judet_sector) + ", " + W.quote(oras) + ", " + W.quote(tara) + ");";
        W.exec(sql_adrese);

        string sql_email = "INSERT INTO EMAIL (UTILIZATORID, ADRESA_EMAIL) VALUES (" + W.quote(utilizatorid) + ", " + W.quote(adresa_email) + ");";
        W.exec(sql_email);

        string sql_telefon = "INSERT INTO TELEFON (UTILIZATORID, NR_TELEFON) VALUES (" + W.quote(utilizatorid) + ", " + W.quote(nr_telefon) + ");";
        W.exec(sql_telefon);

        string sql_logs = "INSERT INTO LOGS (UTILIZATORID, DATA_ORA, SELECTIA, CONT_DESTINATAR, REUSITA) VALUES (" + W.quote(utilizatorid) + ",  NOW(), " + to_string(1) + ", " + to_string(0) + ", " + string(success ? "TRUE" : "FALSE") + ");";
        W.exec(sql_logs);

        W.commit();

        cout << "Utilizator adaugat cu succes!" << endl;
        
    }

    catch (const sql_error &e)
    {
        cerr << "SQL error: " << e.what() << endl;
    }
    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << endl;
    }
}   

void adaugaUtilizatorNou(string& nume, string& prenume, string& dataNasterii, string& cnp, string& strada, string& numar, 
                            string& judet_sector, string& oras, string& tara, string& adresa_email, string& nr_telefon)
{
    cout << "Nume: ";
    cin >> nume;
    cout << "Prenume: ";
    cin >> prenume;
    cout << "Data Nasterii (YYYY-MM-DD): ";
    cin >> dataNasterii;
    cout << "CNP: ";
    cin >> cnp;
    cout << "Strada: ";
    cin >> strada;
    cout << "Numar: ";
    cin >> numar;
    cout << "Judet/Sector: ";
    cin >> judet_sector;
    cout << "Oras: ";
    cin >> oras;
    cout << "Tara: ";
    cin >> tara;
    cout << "E-mail: ";
    cin >> adresa_email;
    cout << "Telefon: ";
    cin >> nr_telefon;
}