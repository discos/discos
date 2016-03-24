#include <assert.h>
#include <iostream>

#include <IRA>

int main()
{
    IRA::CTimer timer;
    ACS::Time elapsed;
    std::cout << "Created timer\n";
    timer.reset();
    IRA::CIRATools::Wait(1,0);
    assert(timer.elapsed() >= 10000000);
    for(int i=0; i<60; i+=10)
    {
        std::cout << "testing timed wait of " << i << " sec.\n";
        timer.reset();
        IRA::CIRATools::Wait(i, 0);
        elapsed = timer.elapsed();
        assert(elapsed >= (ACS::Time)(i * 10000000));
        std::cout << "ok: " << elapsed << '\n';
    }
    std::cout << "all ok\n";
    return 0;
}

