#include <iostream>
#include <string.h>

class a
{
private:
    /* data */
public:
    a(/* args */);
    ~a();

    virtual int test1();
};

a::a(/* args */)
{
}

a::~a()
{
}

int a::test1()
{
    printf("a 1\n");

    return 0;
}

class b : public a
{
public:
    b(/* args */){};
    ~b(){};

    int test1()
    {
        printf("b 1\n");
        return 0;
    }
};

int testVirtual(a* testa){
    testa->test1();
    return 0;
}

int main()
{
    int arr[3] = {0,};
    memset(arr, -1, 3);
    for (int i = 0; i < 3; i ++){
        printf("%d\n", arr[i]);
    }
    // testb->test1();
    return 0;
}