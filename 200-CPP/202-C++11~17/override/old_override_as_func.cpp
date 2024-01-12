class Warning {         // C++98潜在的传统类代码
public:
    void override() {}    // C++98和C++11都合法（且含义相同）
    void final() {}       // C++98和C++11都合法（且含义相同）
    // ...
};

int main() {
    Warning w;
}