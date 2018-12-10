// форсировка решетки в кг/м2 час
// масса подаваемого угля в час, площадь колосниковой решетки
internal inline f64
GetGridForcing(f64 massInKgHour, f64 square)
{
    return (massInKgHour / square);
}

// Bh - сжигаемое топливо в час (кг)
// R  - площадь колосниковой решетки
// U  - напряжение колосниковой решетки
internal inline f64
GetBh(FireChamber fireChamber)
{
    return fireChamber.R * fireChamber.U;
}

internal inline f64
GetBhFact(FireChamber fireChamber, f64 q22, Fuel &fuel)
{
    fuel.u = (100.0 - q22) / 100.0;
    return fuel.u * fireChamber.R * fireChamber.U;
}

internal inline void
CalculateFireChamber(FireChamber &fireChamber)
{
    fireChamber.R = fireChamber.BottomLength * fireChamber.BottomWidth;
    fireChamber.Ht =
        fireChamber.TopLengh * fireChamber.TopWidth +
        fireChamber.RearHeight * fireChamber.TopWidth +
        fireChamber.FrontHeight * fireChamber.TopWidth +
        fireChamber.FrontHeight * fireChamber.TopLengh +
        fireChamber.RearHeight * fireChamber.TopLengh;
}

// Q = Bh * Gb * cp * T
// Q - часовое теплосодержание
// Bh - сжигаемое топливо в час (кг)
// Gb - количество продуктов сгорания (кг на каждый кг топлива)
// cp - средняя теплоемкость продуктов сгорания
// T - температура газов

// Q = Bh * Gb * c * T + Bh * Gh * b * T^2
// Bh * Gb * c = M
// Bh * Gh * b = N
// Q = M * T + M * T^2

// Gb * c = 0.55 * (C / (CO2 + CO)) + 0.0021 * C + 0.0406 * H + 0.0045 * W
// Gb * b = 0.0000445 * (C / (CO2 + CO)) + 0.0000012 * C + 0.0000044 * H + 0.0000005 * W

// a = 1.30-1.60
// a - коэффициент избытка топлива
// a = 1 / (1 - 3.76 * (O2 - 0.5*CO) / N2

// b0 = 2.37 * (H - O/8) / C
// b0 - химическая характеристика топлива
// C = 75-92 (83)
// H = 2.5-5.7 (4)
// O = 1.5-15 (4)

// CO = (21 - b0 * CO2 - (CO2 + O2)) / (0.605 + b0)
// CO - величина недогорания

// 100 = CO2 + CO + O2 + N2

// Q0 = Q1 + (Q2' + Q2'') + Q3 + Q4 + Q5
// Q0 - распологаемое тепло (кал за час работы котла)
// Q1 - используемое тепло
// Q2' - химические потери тепла
// Q2'' - механические потери тепла
// Q3 - потеря тепла с уходящими газами
// Q4 - потеря в окружающую среду
// Q5 - потеря на служебные нужды

// Q0 = Q0' + Q0''

// Q0' = Bh * K
// Q0' - тепло выделяемое при сгорании топлива за час
// K - теплопроизводительность 1 кг топлива (низшая)
// K = 7000-7610 кал
// Bh - количество топлива сгораемого за час в кг

// Q0'' - тепло вносимое в топку с топливом и воздухом
// Q0'' = M * tb + N * tb^2
// tb - температура наружного воздуха

// K = Gb * c * T0 + Gb * b * T0^2
// T0 - теоретическая температура горения

// Q0 - (Q2' + Q2'') = u * (M * T1 + N * T1^2)
// Q0 - (Q2' + Q2'') - фактически выделяемое при сгорании тепло
// u - коэффициент механической полноты сгорания
// u = 0.8-0.88 (~0.84)

// Q2' = 56.9 * C * (CO / (CO2 + CO)) * Bh * u
// Q2'' = 5-30% (~17.5)

// Ht - поверхность нагрева топки
// H - поверхность нагрева всего котла
// T2 - температура при входе газов в отверстия огневой решетки
// T3 - температура газов по выходе из трубчатой части котла

// Q4 - потери на внешнее охлаждение
// Q4 = phi * H0 * (2.2 + 0.21 * V^0.7) * (tk - tb)^(4/3)
// Q4 = phi * H0 * (2.2 + 0.51 * w^0.7) * (tk - tb)^(4/3)
// H0 - наружная поверхность котла
// tk и tb - температуры горячего и холодного источников
// tk = 190
// phi - коэффициент качества изоляции
// phi - от 0.4-1.0
// phi - 0.4 асбестовая изоляция
// phi - 0.48 скелянная вата
// phi - 0.61 частичная изоляция
// phi - 1.0 неизолированный котел
// V - скорость в км/ч
// w - скорость в м/сек

// Q5 - потеря на служебные нужды
// Q5 = 2-5%

// a - коэффициент избытка топлива
internal inline f64
GetCO2(f64 a)
{
    return Map(a, 1.0, 2.0, 18.35, 9.48);
}

// a - коэффициент избытка топлива
internal inline f64
GetO2(f64 a)
{
    return Map(a, 1.0, 1.7, 2.0, 9.3);
}

internal inline void
GetAlpha(Fuel &fuel)
{
    // b0 = 2.37 * (H - O/8) / C
    fuel.alpha = 1.0 / (1.0 - 3.76 * (fuel.O2 / fuel.N2));
}

// beta0 - химическая характеристика топлива
internal inline void
GetBeta0(Fuel &fuel)
{
    // b0 = 2.37 * (H - O/8) / C
    fuel.beta0 = (2.37 * ((fuel.H - fuel.O / 8.0) / fuel.C));
}

internal inline void
GetCO(Fuel &fuel)
{
    // CO = (21 - b0 * CO2 - (CO2 + O2)) / (0.605 + b0)

    auto top = 21.0 - fuel.beta0 * fuel.CO2 - (fuel.CO2 + fuel.O2);
    auto bottom = 0.605 + fuel.beta0;
    fuel.CO = top / bottom;
}

internal inline void
GetCO2(Fuel &fuel)
{
    fuel.CO2 = (79.0 / ((1.0 + fuel.beta0) * (4.76 * fuel.alpha - 1.0) - (fuel.alpha - 1.0)));
}

internal inline void
GetO2(Fuel &fuel)
{
    auto top = (79.0 + 100.0 * fuel.beta0) * (fuel.alpha - 1.0);
    auto bottom = (1.0 + fuel.beta0) * (4.76 * fuel.alpha - 1.0) - (fuel.alpha - 1.0);
    fuel.O2 = top / bottom;
}

internal inline void
GetN2(Fuel &fuel)
{
    fuel.N2 = 100.0 - (fuel.CO2 + fuel.O2);
}

// a - коэффициент избытка топлива
// Bh - количество топлива сгораемого за час в кг
internal inline f64
GetM(f64 Bh, Fuel fuel)
{
    auto Gbc = 0.55 * (fuel.C / (fuel.CO2 + fuel.CO)) + 0.0021 * fuel.C + 0.0406 * fuel.H + 0.0045 * fuel.W;

    return Gbc * Bh;
}

// a - коэффициент избытка топлива
// Bh - количество топлива сгораемого за час в кг
internal inline f64
GetN(f64 Bh, Fuel fuel)
{
    auto Gbb = 0.0000445 * (fuel.C / (fuel.CO2 + fuel.CO)) + 0.0000012 * fuel.C + 0.0000044 * fuel.H + 0.0000005 * fuel.W;

    return Gbb * Bh;
}

internal inline void
GetHeatCoefficient(HeatCoefficient &heatCoefficient, Fuel fuel, f64 Bh)
{
    heatCoefficient.M = GetM(Bh, fuel);
    heatCoefficient.N = GetN(Bh, fuel);
}

// T0 - теоретическая температура горения
// a - коэффициент избытка топлива
internal inline f64
GetT0(f64 a)
{
    return Map(a, 1.0, 2.0, 2148.0, 1304.0);
}

// Q0 - распологаемое тепло
// a - коэффициент избытка топлива
// Bh - количество топлива сгораемого за час в кг
// tb - температура наружного воздуха
internal inline f64
GetQ0(f64 Bh, f64 tb, Fuel fuel, HeatCoefficient heatCoefficient)
{
    // Q0 = Q0' + Q0''
    // Q0' = Bh * K
    // Q0'' = M * tb + N * tb^2

    return (Bh * fuel.K + heatCoefficient.M * tb + heatCoefficient.N * tb * tb);
}

// Q1  - полезное тепло
// Bt  - расчетный расход пара на работу машины в кг/час
// Bk  - полная часова производительность котла
// lY  - теплосодержание пара по выходе в кал/кг
// lY  - теплосодержание пара в котле в кал/кг
// phi - температура питательной воды
internal inline f64
GetQ1(f64 Bt, f64 Bk, f64 lY, f64 lK, f64 phi)
{
    return (Bt * (lY - phi) + (Bk - Bt) * (lK - phi));
}

// Q2' - химические потери тепла
// a - коэффициент избытка топлива
// b0 - химическая характеристика топлива
// Bh - количество топлива сгораемого за час в кг
internal inline f64
GetQ21(f64 BhFact, Fuel fuel)
{
    return (56.9 * fuel.C * (fuel.CO / (fuel.CO2 + fuel.CO)) * BhFact);
}

// Q2'' - механические потери тепла
// Q0   - распологаемое тепло
// q22  - суммарная потеря от уноса, слака и провала угля в %
internal inline f64
GetQ22(f64 Q0, f64 q22)
{
    return (Q0 * q22 / 100.0);
}

// Q3 - потеря тепла с уходящими газами
// a - коэффициент избытка топлива
// Bh - количество топлива сгораемого за час в кг
// Q0 - распологаемое тепло
// T3 - температура газов по выходе из трубчатой части котла
internal inline f64
GetQ3(f64 Bh, f64 Q0, f64 T3, Fuel fuel, HeatCoefficient heatCoefficient)
{
    auto q3 = heatCoefficient.M / fuel.K * (heatCoefficient.M * T3 + heatCoefficient.N * T3 * T3) * 100.0;
    return Q0 * q3;
}

internal inline f64
GetQ3(f64 T3, HeatCoefficient heatCoefficient)
{
    return (heatCoefficient.M * T3 + heatCoefficient.N * T3 * T3);
}

// Q4 - потери на внешнее охлаждение
// phi - коэффициент качества изоляции
// H0 - наружная поверхность котла м^2
// V - скорость в км/ч
// tb - температура наружного воздуха
internal inline f64
GetQ4(f64 phi, f64 H0, f64 V, f64 tb, f64 tk)
{
    // Q4 = 2.2 * phi * H0 * (tk - tb)^(4/3)
    return phi * H0 * (2.2 + 0.21 * pow(V, 0.7)) * pow(tk - tb, 4.0 / 3.0);
}

// Q4 - потери на внешнее охлаждение
// k  - потеря в % от Q0
internal inline f64
GetQ4(f64 Q0, f64 k)
{
    return Q0 * k / 100.0;
}

// Q5 - потеря на служебные нужды
internal inline f64
GetQ5(f64 Q0)
{
    // Q5 = 2-5%
    return (Q0 * 0.035);
}

// T1   - Действительная температура горения
// Q0   - распологаемое тепло
// Q2'  - химические потери тепла
// Q2'' - механические потери тепла
// a    - коэффициент избытка топлива
// Bh   - количество топлива сгораемого за час в кг
internal inline f64
GetT1(f64 Q0, f64 Q21, f64 Q22, f64 Bh, Fuel fuel, HeatCoefficient heatCoefficient)
{
    //N * T^2 + M * T - Q / 0.84 = 0
    auto Q = (Q0 - (Q21 + Q22)) / 0.84;
    return SolveQuadratic(heatCoefficient.N, heatCoefficient.M, -Q);
}

// T2 - температура при входе газов в отверстия огневой решетки (43)
// Bh - количество топлива сгораемого за час в кг
// Ht - поверхность нагрева огневой коробки
internal inline f64
GetT2(f64 Bh, f64 Ht, Fuel fuel)
{
    const f64 A = 1350.0;

    auto top = (Bh * fuel.K) / Ht + 4400.0;
    auto bottom = (Bh * fuel.K) / Ht + 223000.0;

    auto result = A * Root(top / bottom, 1.6);

    return result;
}

// H - поверхность нагрева труб
// n - число труб
// l - длина трубы
internal inline f64
GetHPipes(PipeDiameter d, u16 n, f64 l)
{
    return PI * d.DOut * n * l;
}

internal inline f64
GetHydraulicRadius(PipeDiameter d)
{
    return (d.DIn / 4.0);
}

// T3 - температура газов по выходе из трубчатой части котла
// d  - диаметр дымогарной трубы
// L  - длина трубчатой части котла
// Bh - количество топлива сгораемого за час в кг
// Hk - поверхность нагрева котла
// Hi - поверхность нагрева пароперегревателя
internal inline f64
GetT3(PipeDiameter d, u16 n, f64 L, f64 Bh, f64 Hk, Fuel fuel)
{
    auto Hi = GetHPipes(d, n, L);
    auto H = Hk + Hi;               // полная поверхность нагрева
    auto r = GetHydraulicRadius(d); // гидравлический радиус
    auto A = pow(r / 0.0105, 0.15) * (710.0 + 332000.0 / (L / r + 105.0));
    auto top = (Bh * fuel.K) / H + 4400.0;
    auto bottom = (Bh * fuel.K) / H + 223000.0;

    return (A * Root(top / bottom, 1.6));
}

internal inline f64
ConvertCalHourToWatts(f64 calhour)
{
    return calhour * 0.001162;
}

// Q     - переход тепла от газа к материалу стенки
// h     - продолжительность явления теплообмена в часах
// H     - площадь поверхности стенки
// a1    - коэффициент теплопередачи от газа материалу стенки
// T     - температура газа
// Phi   - температура нижней поверхности стенки
internal inline f64
GetQGasToWall(f64 h, f64 dH, f64 T, f64 t, f64 k)
{
    return (k * h * dH * (T - t));
}

// betaS - слой сажи в метрах
// betaN - слой накипи в метрах
internal inline f64
GetTOfWallOnWaterSide(f64 T, f64 t, f64 betaN, f64 betaS)
{
    const f64 a1 = 150.0;
    const f64 a2 = 2000.0;
    const f64 beta = 0.01;
    const f64 gamma = 50.0;
    const f64 gammaS = 0.1;
    const f64 gammaN = 2.0;
    auto QhH = 1.0 / (1.0 / a1 + 1.0 / a2 + beta / gamma + betaN / gammaN + betaS / gammaS) * (T - t);

    return (t + (QhH / a2));
}

// betaS - слой сажи в метрах
// betaN - слой накипи в метрах
internal inline f64
GetTOfWallOnGasSide(f64 T, f64 t, f64 betaN, f64 betaS)
{
    const f64 a1 = 150.0;
    const f64 a2 = 2000.0;
    const f64 beta = 0.01;
    const f64 gamma = 50.0;
    const f64 gammaS = 0.1;
    const f64 gammaN = 2.0;
    auto QhH = 1.0 / ((1.0 / a1 + 1.0 / a2) + beta / gamma + betaN / gammaN + betaS / gammaS) * (T - t);

    return (T - (QhH / a1));
}

// Zt  - полезная интенсивность парообразования
//       (кг пара, снимаемое в 1 час с 1м2 испаряющей поверхности)
// Y   - интенсивность горения топлива на колосниковой решетке
// R   - площадь колосниковой решетки
// Hk  - испаряющая поверхность нагрева
// lY  - состояние пара
// nk  - степень использования топлива
internal inline f64
GetZt()
{
    return 0.0;
}

// H  - высота парового пространства (83)
// Dc - внутренни диаметр барабана
internal inline f64
GetH(f64 Dc)
{
    constexpr f64 k = (1.0 / 4.3);
    return k * Dc;
}

// Дымогарные трубы

internal inline f64
GetKdHd(f64 b, f64 M, f64 N, f64 T2, f64 Tk, f64 tk, f64 T3d)
{
    return ((1.0 - b) * ((M + 2.0 * N * tk) * log((T2 - Tk) / (T3d - tk)) + 2.0 * N * (T2 - T3d)));
}

internal inline f64
GetKd(f64 Wd, f64 rd)
{
    return (6.0 + 2.45 * pow(Wd, 0.7) * (pow(0.0115 / rd, 0.214)));
}

internal inline f64
GetWd(f64 L0, f64 a, f64 Bh, f64 u, f64 omegaD, f64 T2, f64 T3d, f64 b)
{
    return (0.8425 * ((L0 * a * Bh * u) / (10000000.0 * omegaD)) * ((T2 + T3d) / 2.0 + 273) * (1.0 - b));
}

// Qt  - тепло проходящее в котел через топочную
internal inline f64
GetQt(f64 Q0, f64 Q21, f64 Q22, f64 T2, HeatCoefficient heatCoefficient)
{
    return Q0 - Q21 - Q22 - (heatCoefficient.M * T2 + heatCoefficient.N * T2 * T2);
}

internal inline void
CalculateFuel(Fuel &fuel)
{
    fuel.C = 80;
    fuel.H = 3.1;
    fuel.S = 1.9;
    fuel.O = 3.1;
    fuel.N = 1.1;
    fuel.W = 3.1;
    fuel.A = 7.6;
    fuel.K = 7203;

    fuel.alpha = 1.35;

    fuel.O2 = 6.2;
    fuel.N2 = 79.6;
    fuel.CO = 1.5;
    fuel.CO2 = 12.7;

    GetBeta0(fuel);
}

// L   - длина трубы
// r   - гидравлический радиус
// sqr - площадь живого сечения трубы
internal inline f64
GetBeta(f64 Ld, f64 rd, f64 sqrD, f64 Lg1, f64 rg1, f64 sqrG1, f64 Lg2, f64 rg2, f64 sqrG2)
{
    auto a = Ld / (rd * sqrD * sqrD);
    auto b = Lg1 / (rg1 * sqrG1 * sqrG1);
    auto c = Lg2 / (rg2 * sqrG2 * sqrG2);

    auto top = a - sqrt((1.33 * b + 0.96 * c) * a);
    auto bottom = (a - 1.33 * b - 0.96 * c);

    printf("top \t\t\t\t\t\t%.2lf\n", top);
    printf("bottom \t\t\t\t\t\t%.2lf\n", bottom);

    return (top / bottom);
}

// L0 - теоретический расход воздуха для сжигания 1 кг топлива
internal inline f64
GetL0(Fuel fuel)
{
    return (1.0 / 23.6 * (8.0 / 3.0 * fuel.C + 8 * fuel.H + fuel.S - fuel.O));
}

// Lv - действительный расход воздуха для сжигания 1 кг топлива
internal inline f64
GetLv(f64 L0, Fuel fuel)
{
    return L0 * fuel.alpha;
}

// Ki -
internal inline f64
GetKi(f64 beta, f64 Bh, f64 rz2, f64 omegaZ2, Fuel fuel)
{
    auto x = pow((beta * Bh * fuel.u * fuel.K) / (10000.0 * omegaZ2), 0.7);
    auto y = pow(0.0115 / rz2, 0.214);
    return (6.0 + 0.24 * x * y);
}

// Tabs - средняя абсолютная температура газов в дымогарных трубах
internal inline f64
GetTabs(f64 T2, f64 T3)
{
    return ((T2 + T3) / 2.0 + 273.0);
}

// O - площадь живого сечения трубы
internal inline f64
GetPipeSquare(PipeDiameter d)
{
    auto dSq = d.DIn * d.DIn;
    return ((PI * dSq) / 4.0);
}

//v - удельный объем протекающих по трубкам продуктов сгорания
internal inline f64
GetV(f64 Tabs)
{
    return ((29.27 * Tabs) / 10330.0);
}

// omega - скорость протекания газов по дымогарным трубам
internal inline f64
GetOmega(Fuel fuel, PipeDiameter d, f64 Tabs, f64 L0, f64 Bh)
{
    auto Od = GetPipeSquare(d);
    auto v = GetV(Tabs);
    auto top = (L0 * fuel.alpha + 1.0) * Bh * v;
    auto bottom = 3600.0 * Od;
    return (top / bottom);
}

// k - коэффициент теплопередачи
internal inline f64
GetK(PipeDiameter d, f64 omega)
{
    auto r = GetHydraulicRadius(d);
    return ((6.0 + 2.45 * pow(omega, 0.7)) * pow(0.0115 / r, 0.214));
}

// k - коэффициент теплопередачи
internal inline f64
GetK(HeatCoefficient heatCoefficient, f64 Hd, f64 tk, f64 T2, f64 T3)
{
    auto a = (heatCoefficient.M + 2.0 * heatCoefficient.N * tk);
    auto b = log((T2 - tk) / (T3 - tk));
    auto c = 2.0 * heatCoefficient.N * (T2 - T3);
    return (a * b + c) / Hd;
}