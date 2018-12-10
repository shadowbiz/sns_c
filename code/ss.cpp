#include "ss.h"

#include "ss_boiler.cpp"
#include "ss_gear.cpp"

extern "C" CALCULATE(Calculate)
{
    const f64 U = 550.0; // напряжение колосниковой решетки

    const f64 t = 0.0; // температура окружающего воздуха °C
    const f64 v = 0.0; // скорость км/ч

    const f64 q22 = 36.0; // суммарная потеря от уноса, шлака и провала угля в %
    const f64 tk = 190.0;

    FireChamber fireChamber = {};

    fireChamber.TopLengh = MillimeterToMeter(2222);
    fireChamber.BottomLength = MillimeterToMeter(2278);

    fireChamber.TopWidth = MillimeterToMeter(1333);
    fireChamber.BottomWidth = MillimeterToMeter(1028);

    fireChamber.FrontHeight = MillimeterToMeter(1815);
    fireChamber.RearHeight = MillimeterToMeter(1605);

    /*
    fireChamber.TopLengh     = MillimeterToMeter(2649);
    fireChamber.BottomLength = MillimeterToMeter(2744);
    
    fireChamber.TopWidth     = MillimeterToMeter(1376);
    fireChamber.BottomWidth  = MillimeterToMeter(1016);
    
    fireChamber.FrontHeight  = MillimeterToMeter(1800);
    fireChamber.RearHeight   = MillimeterToMeter(1606);
    */

    fireChamber.U = 550;
    CalculateFireChamber(fireChamber);

    //const f64 R = 2.8;       // площадь колосниковой решетки
    //const f64 Ht = 15.6;     // поверхность нагрева огневой коробки

    PipeDiameter dd = {}; // дымогарная труба
    dd.DOut = MillimeterToMeter(51.0);
    dd.DIn = MillimeterToMeter(46.0);

    const f64 Ld = MillimeterToMeter(4550); // длина труб дымогарных
    const u16 nd = 210;                     // число дымогарных труб

    auto Hd = GetHPipes(dd, nd, Ld);

    Fuel fuel = {};
    CalculateFuel(fuel);

    auto Bh = GetBh(fireChamber);                    // сжигаемое топливо в час (кг)
    auto BhFact = GetBhFact(fireChamber, q22, fuel); // фактически сжигаемое топливо в час (кг)

    HeatCoefficient heatCoefficient = {};
    GetHeatCoefficient(heatCoefficient, fuel, BhFact);

    auto Q0 = GetQ0(Bh, t, fuel, heatCoefficient);
    auto Q21 = GetQ21(BhFact, fuel);
    auto Q22 = GetQ22(Q0, q22);

    auto T1 = GetT1(Q0, Q21, Q22, BhFact, fuel, heatCoefficient);
    auto T2 = GetT2(BhFact, fireChamber.Ht, fuel);

    //auto Q4 = GetQ4(.4, 51.9, v, t);
    auto Q4 = GetQ4(Q0, 1); // 1% потерь от Q0

    auto L0 = GetL0(fuel); // теоретический расход воздуха для сжигания 1 кг топлива

    printf("Коэффициент избытка топлива alpha\t\t%.2lf\n", fuel.alpha);
    printf("Химическая характеристика топлива b0 \t\t%.2lf\n", fuel.beta0);

    /*
    printf("Коэффициент уравнения тепла M \t\t\t%.2lf\n", heatCoefficient.M);
    printf("Коэффициент уравнения тепла N \t\t\t%.5lf\n", heatCoefficient.N);
    */

    printf("Теоретический расход воздуха L0\t\t\t%.2lf кг\n", L0);

    printf("Сжигаемое топливо в час  Bh \t\t\t%.2lf кг\n", Bh);
    printf("Сжигаемое топливо в час по факту \t\t%.2lf кг\n", BhFact);

    printf("Действительная температура горения T1\t\t%.2lf °C\n", T1);
    printf("Температура при входе газов в отверстия T2\t%.2lf °C\n", T2);

    /*
    const f64 betaN = 0.000001; // слой сажи в метрах
    const f64 betaS = 0.000001; // слой накипи в метрах
    
    auto TWaterSide = GetTOfWallOnWaterSide(1300.0, 190.0, betaN, betaS);
    auto TGasSide = GetTOfWallOnGasSide(1300.0, 190.0, betaN, betaS);
    
    printf("Температура стенки со стороны газов\t\t%.2lf °C\n", TGasSide);
    printf("Температура стенки со стороны воды\t\t%.2lf °C\n", TWaterSide);
    */

    auto T3 = GetT3(dd, nd, Ld, BhFact, fireChamber.Ht, fuel);
    auto Tabs = GetTabs(T2, T3);

    auto Q3 = GetQ3(T3, heatCoefficient);

    // Qt  - тепло проходящее в котел через топочную
    auto Qt = GetQt(Q0, Q21, Q22, T2, heatCoefficient);

    printf("Температура газов на выходе котла T3\t\t%.2lf °C\n", T3);
    printf("Ср. абс. температура газов в д.трубах Tabs\t%.2lf °C\n", Tabs);
    printf("Располагаемое тепло Q0 \t\t\t\t%.2lf кал/час \t\t%.1lf%%\n", Q0, Q0 / Q0 * 100);
    printf("Химические потери тепла Q2'\t\t\t%.2lf кал/час \t\t%.1lf%%\n", Q21, Q21 / Q0 * 100);
    printf("Механические потери тепла Q2''\t\t\t%.2lf кал/час \t\t%.1lf%%\n", Q22, Q22 / Q0 * 100);
    printf("Потеря тепла с уходящими газами Q3\t\t%.2lf кал/час \t\t%.1lf%%\n", Q3, Q3 / Q0 * 100);
    printf("Потери на внешнее охлаждение Q4\t\t\t%.2lf кал/час \t\t%.1lf%%\n", Q4, Q4 / Q0 * 100);

    printf("Тепло проходящее в котел через топочную Qt\t%.2lf кал/час \t\t%.1lf%%\n", Qt, Qt / Q0 * 100);

    auto Hi = fireChamber.Ht + Hd;

    printf("Площадь колосниковой решетки R\t\t\t%.2lf\n", fireChamber.R);
    printf("Поверхность нагрева огневой коробки Ht\t\t%.2lf м2\n", fireChamber.Ht);
    printf("Поверхность нагрева дымогарных труб Hd\t\t%.2lf м2\n", Hd);
    printf("Полная испаряющая поверхность Hi\t\t%.2lf м2\n", Hi);

    auto omega = GetOmega(fuel, dd, Tabs, L0, BhFact);
    auto k1 = GetK(dd, omega);
    auto k2 = GetK(heatCoefficient, Hd, tk, T3, T3);

    auto rd = GetHydraulicRadius(dd);
    auto sqrD = GetPipeSquare(dd);

    //auto bb = GetBeta(Ld, rd, sqrD, 0, 0, 0, 0, 0, 0);

    printf("k1 \t\t\t\t\t\t%.2lf\n", k1);
}
