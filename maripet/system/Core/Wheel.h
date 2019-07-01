
class Wheel{
  public:
    void Setup(int, int,int,int,int,int);
    void Go();
    void Stop();
    void Back();
    void Right();
    void Left();
    void Speed(int);
    
  private:
    int pinR0;
    int pinL0;
    int pinR1;
    int pinL1;
    int pinR_pwm;
    int pinL_pwm;
    
};
