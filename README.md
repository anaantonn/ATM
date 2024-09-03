## ATM APPLICATION

An application made to practice my C++ programming and also familiarize myself with SQL (for this I used postresql).
I created a database (in Postgres) that stores each new account created, at it automatically generates a PIN
and an account number.

## Features

Upon running the program, the user is prompted to either create anew account, to enter an existing account or exit the application.
When choosing to create a new account the user has to input hte requested data, which is automatically stored in the database tables.
If the second option is selected then the user can either see the account balance, withdraw, deposit or transfer money.


## Instalation

1. To compile the program:

    ```bash
    g++ src/INTERFATA.cpp -lpqxx -lpq -o main.out
    ```
2. To run the program:

    ```bash
    ./main.out
    ```
