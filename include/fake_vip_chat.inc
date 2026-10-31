
// ============================================================================
// FAKE VIP CHAT (чисто визуальная имитация активности в /v)
// - никаких ботов в игре: ни слотов, ни NPC, ни записей в БД, ни MySQL-запросов
// - одно сообщение раз в FVC_DELAY_MIN..FVC_DELAY_MIN+FVC_DELAY_JITTER мс
// - ник, ID, префикс, объявление и номер телефона каждый раз случайные
// - рассылается только онлайн-игрокам с включённым VIP-чатом (как обычный /v)
// - для отключения/включения: RCON-админу команда /fakevip
// ============================================================================
#define FVC_DELAY_MIN       10000   // минимальная пауза между сообщениями, мс
#define FVC_DELAY_JITTER    5000    // + случайная добавка 0..N мс, чтобы не было "по метроному"
#define FVC_COLOR           0xE8F808FF

enum E_FVC_CAR { FVC_CAR_NAME[40], FVC_CAR_PRICE };

new bool:g_fvc_enabled = true;

new const g_fvc_first[][12] = {
    "Artem",
    "Maksim",
    "Dmitriy",
    "Ivan",
    "Nikita",
    "Egor",
    "Kirill",
    "Andrey",
    "Denis",
    "Roman",
    "Pavel",
    "Sergey",
    "Alexey",
    "Vladimir",
    "Danil",
    "Timur",
    "Ruslan",
    "Anton",
    "Vlad",
    "Stas",
    "Oleg",
    "Igor",
    "Artur",
    "Daniil",
    "Mikhail",
    "Matvey",
    "Yaroslav",
    "Lev",
    "Evgeniy",
    "Vitaliy",
    "Georgiy",
    "Gleb",
    "Valera",
    "Bogdan",
    "Semen",
    "Arseniy",
    "Zakhar",
    "Nikolay",
    "Viktor",
    "Konstantin",
    "Grigoriy",
    "Fedor",
    "Rodion",
    "Eldar",
    "Rinat",
    "Marat",
    "Alan",
    "Kostya",
    "Ilya"
};
new const g_fvc_last[][16] = {
    "Volkov",
    "Morozov",
    "Sokolov",
    "Kuznetsov",
    "Popov",
    "Lebedev",
    "Smirnov",
    "Novikov",
    "Fedorov",
    "Orlov",
    "Belov",
    "Kozlov",
    "Zaytsev",
    "Pavlov",
    "Semenov",
    "Golubev",
    "Vinogradov",
    "Bogdanov",
    "Vorobev",
    "Nikitin",
    "Medvedev",
    "Egorov",
    "Tarasov",
    "Belyaev",
    "Komarov",
    "Kiselev",
    "Ilyin",
    "Gusev",
    "Titov",
    "Kuzmin",
    "Kudryavtsev",
    "Baranov",
    "Kulikov",
    "Alekseev",
    "Stepanov",
    "Yakovlev",
    "Sorokin",
    "Romanov",
    "Zakharov",
    "Borisov",
    "Korolev",
    "Gerasimov",
    "Ponomarev",
    "Grigorev",
    "Lazarev",
    "Ershov",
    "Sobolev",
    "Ryabov",
    "Polyakov",
    "Tsvetkov",
    "Danilov",
    "Zhukov",
    "Frolov",
    "Zhuravlev",
    "Nikolaev",
    "Krylov",
    "Maksimov",
    "Sidorov",
    "Osipov",
    "Belousov",
    "Fedotov",
    "Dorofeev",
    "Matveev",
    "Bobrov",
    "Dmitriev",
    "Kalinin",
    "Anisimov",
    "Petukhov",
    "Antonov",
    "Timofeev",
    "Nikiforov",
    "Veselov",
    "Filippov",
    "Markov",
    "Bolshakov",
    "Sukhanov",
    "Mironov",
    "Shiryaev",
    "Aleksandrov",
    "Konovalov",
    "Shestakov",
    "Kazakov",
    "Efimov",
    "Denisov",
    "Gromov",
    "Fomin",
    "Davydov",
    "Melnikov",
    "Shcherbakov",
    "Blinov",
    "Kolesnikov",
    "Karpov",
    "Afanasev",
    "Vlasov",
    "Maslov",
    "Isakov",
    "Tikhonov",
    "Aksenov",
    "Gavrilov",
    "Rodionov",
    "Kotov",
    "Gorbunov",
    "Kudryashov",
    "Bykov",
    "Zuev",
    "Tretyakov",
    "Savelev",
    "Panov",
    "Rybakov",
    "Suvorov",
    "Abramov",
    "Voronov",
    "Mukhin",
    "Arkhipov",
    "Trofimov",
    "Martynov",
    "Emelyanov",
    "Gorshkov",
    "Chernov",
    "Ovchinnikov",
    "Seleznev",
    "Panfilov",
    "Kopylov",
    "Mikheev",
    "Galkin",
    "Nazarov",
    "Lobanov",
    "Lukin",
    "Belyakov",
    "Potapov",
    "Nekrasov",
    "Khokhlov",
    "Zhdanov",
    "Naumov",
    "Shilov",
    "Vorontsov",
    "Ermakov",
    "Drozdov",
    "Ignatev",
    "Savin",
    "Loginov",
    "Safonov",
    "Kapustin",
    "Kirillov",
    "Moiseev",
    "Eliseev",
    "Koshelev",
    "Kostin",
    "Gorbachev",
    "Orekhov",
    "Efremov",
    "Isaev",
    "Evdokimov",
    "Kalashnikov",
    "Kabanov",
    "Noskov",
    "Yudin",
    "Kulagin",
    "Lapin",
    "Prokhorov",
    "Nesterov",
    "Kharitonov",
    "Agafonov",
    "Muravev",
    "Larionov",
    "Fedoseev",
    "Zimin",
    "Pakhomov",
    "Shubin",
    "Ignatov",
    "Filatov",
    "Krasnov",
    "Kharlamov",
    "Gordeev"
};

// авто взяты из g_vehicle_info, цена - гос. стоимость (дальше рандомится +-)
new const g_fvc_cars[][E_FVC_CAR] = {
    {"VAZ 2108", 170000},
    {"VAZ 2109", 180000},
    {"VAZ 2110", 190000},
    {"VAZ 2114", 195000},
    {"VAZ 2115", 210000},
    {"VAZ 2172", 300000},
    {"BMW M3 E30", 300000},
    {"Niva Urban", 350000},
    {"Lada Granta", 370000},
    {"VAZ 2112", 420000},
    {"Toyota Mark II", 420000},
    {"Nissan Skyline R34", 680000},
    {"Mercedes-Benz E420 W210", 700000},
    {"Lada Vesta", 700000},
    {"Renault Logan", 716000},
    {"Audi A4", 750000},
    {"Lada Vesta SW", 764000},
    {"BMW M5 E39", 800000},
    {"Kia Rio", 815000},
    {"Hyundai Solaris 2021", 830000},
    {"Volkswagen Polo", 850000},
    {"Skoda Rapid", 870000},
    {"Mercedes-Benz S600 W140", 900000},
    {"BMW E38 750i", 950000},
    {"BMW M3 E46", 1050000},
    {"Acura TSX", 1150000},
    {"Uaz Patriot", 1200000},
    {"BMW M3 E92", 1250000},
    {"BMW E38 L7", 1360000},
    {"Volkswagen Golf GTI", 1400000},
    {"ИЖ Юпитер", 1500000},
    {"BMW X5 E53", 1500000},
    {"Ford Focus RS", 1800000},
    {"Ducati SuperSport S", 1830000},
    {"Mitsubishi Lancer Evo X", 2000000},
    {"Toyota Camry 2016", 2000000},
    {"BMW M5 E60", 2100000},
    {"Subaru WRX STI", 2200000},
    {"Toyota Camry 3.5", 2400000},
    {"Kia K5", 2550000},
    {"Ford Mustang GT", 2600000},
    {"Toyota Supra A80", 2700000},
    {"Mercedes-Benz A45 AMG", 2800000},
    {"Ducati XDiavel S", 3000000},
    {"Alfa Romeo Giulia", 3000000},
    {"Infiniti Q60S", 3200000},
    {"Volvo V60", 3500000},
    {"Volkswagen Touareg R-Line", 3900000},
    {"Volkswagen Multivan T6", 4000000},
    {"Honda NSX-R NA2", 4140000},
    {"Volvo XC90", 4200000},
    {"Toyota GT-86", 4200000},
    {"KIA Stinger", 4200000},
    {"Jeep Grand Cherokee Trackhawk SRT", 4250000},
    {"Yamaha FZ-10", 4500000},
    {"Infiniti Q50", 4500000},
    {"Chevrolet Camaro ZL1", 4600000},
    {"BMW 3-Series G20", 4700000},
    {"Lincoln Continental", 4800000},
    {"BMW Z4 M40i", 4900000},
    {"Yamaha YZF-R6", 5000000},
    {"Bentley Bentayga", 5200000},
    {"Jeep Wrangler JL", 5300000},
    {"BMW M4 F84", 5500000},
    {"Ford Raptor F-150", 5757000},
    {"Toyota Supra A90", 5900000},
    {"Dodge Demon SRT", 6100000},
    {"BMW M2 CS", 6170000},
    {"Mercedes-Benz V-Class W447", 6300000},
    {"Ferrari 612 Scaglietti", 6680000},
    {"Mercedes-Benz GT63s", 7000000},
    {"Audi Q7", 7000000},
    {"BMW S 1000 RR", 7000000},
    {"Audi Q7 2020", 7175000},
    {"Cadillac Escalade", 7200000},
    {"Dodge Charger V2", 7250000},
    {"Jaguar F-Pace", 7450000},
    {"BMW M5 F10", 7700000},
    {"Toyota Land Cruiser 200", 7800000},
    {"Nissan GT-R R35", 7900000},
    {"Suzuki GSX-R750", 8000000},
    {"BMW X6M F16", 8200000},
    {"Porsche Panamera S", 8400000},
    {"BMW X6 M F96", 8450000},
    {"Audi RS6", 8500000},
    {"Honda NSX 2017", 8600000},
    {"Mercedes-Benz C63 AMG", 8600000},
    {"BMW M6 F12", 8700000},
    {"Chevrolet Tahoe RST", 8900000},
    {"Lexus LX500", 9000000},
    {"Porsche 911 Carrera S", 9000000},
    {"Mercedes-Benz GLS 400d", 9150000},
    {"BMW M5 F90", 9500000},
    {"Audi RS7 Sport", 9500000},
    {"Audi RS7 C8", 9650000},
    {"Jaguar F-Type", 9800000},
    {"Audi RS6 C7", 10000000},
    {"Range Rover SVR", 10000000},
    {"Kawasaki Ninja H2R", 10000000},
    {"BMW M5 F90 2021", 10250000},
    {"Mercedes-Benz C63s", 10300000},
    {"Cadillac Escalade SUV", 10370000},
    {"BMW X7 M50i", 10500000},
    {"Audi R8 V10", 11000000},
    {"Mercedes-Benz CLS63 AMG", 11000000},
    {"Mercedes-Benz E63s", 11450000},
    {"Lexus LX 570", 11456000},
    {"Cadillac Escalade 2021", 11470000},
    {"Range Rover Evoque", 11500000},
    {"BMW I8 EDrive", 12600000},
    {"BMW I8 Roadster", 12600000},
    {"BMW X5M G05", 13000000},
    {"Mercedes-Benz SL63 2023", 13450000},
    {"Mercedes-Benz GT-R", 13500000},
    {"McLaren 600LT", 14000000},
    {"BMW 7-Series 750Li", 14300000},
    {"Porsche Cayenne S", 14700000},
    {"Audi RS6 C8", 14800000},
    {"Range Rover Velar", 15000000},
    {"Lamborghini Urus", 15300000},
    {"Tesla Model S", 15500000},
    {"BMW M760Li xDrive", 16400000},
    {"Lamborghini Huracan", 16500000},
    {"Chevrolet Corvette C8", 17000000},
    {"Audi A8 2022", 17000000},
    {"Mercedes-Benz G65 AMG", 17050000},
    {"BMW M8 F92", 17200000},
    {"Lamborghini Urus Venum", 17350000},
    {"Ferrari 488 GTB", 17500000},
    {"Porsche Taycan Turbo S", 18300000},
    {"Aston Martin DB11", 18500000},
    {"BMW M5 Competition Asco", 19000000},
    {"BMW M8 F93", 19000000},
    {"BMW M8 F93 Gran Coupe", 19000000},
    {"Mercedes-Benz 300SL Coupe", 19500000},
    {"Lamborghini Aventador S", 20000000},
    {"Tesla Model X", 20500000},
    {"Mercedes-Benz G63 AMG", 23500000},
    {"Mercedes-Benz EQS580", 24000000},
    {"Mercedes-Benz Maybach S650", 25000000},
    {"Ferrari 488 Pista", 26000000},
    {"Rolls-Royce Wraith", 30000000},
    {"Aurus Senat", 30000000},
    {"Rolls-Royce Cullinan", 40000000},
    {"Rolls-Royce Phantom", 50000000},
    {"Bentley Continental GT", 54000000},
    {"BMW M1", 70000000},
    {"Bugatti Atlantic", 75000000},
    {"Bentley Mulliner Becalar", 125000000},
    {"Bugatti Chiron", 140000000},
    {"Bugatti Divo", 380000000},
    {"Bugatti La Noire", 1000000000}
};

// куда ищут дом (с предлогом) и в каком диапазоне цен, руб.
new const g_fvc_house_place[][] = {
    "в Корякино", "в Коряке", "в Нижегородске", "в Бусаево", "на Рублёвке"
};
new const g_fvc_house_min[] = { 30000000,  30000000,  60000000,  25000000, 250000000 };
new const g_fvc_house_max[] = { 90000000,  90000000, 160000000,  80000000, 600000000 };

// тир A: 1 - 1,5ккк | тир B: 100 - 200кк | тир C (магазин): 100 - 400к
new const g_fvc_acs_a[][] = {
    "Крылья бабочки",
    "Череп воробья",
    "Реактивный ранец"
};
new const g_fvc_acs_b[][] = {
    "Шляпа тьмы",
    "Пиратская шляпа",
    "Шляпа ночи",
    "Обезьяна",
    "Меч смерти",
    "Меч тёмных сил",
    "Меч короля пиратов",
    "Подзорная труба",
    "Пистолет чёрных душ",
    "Пистолет чёрной смерти",
    "Легендарная шляпа",
    "Фонарь пирата",
    "Кленовая корона",
    "Маска пчелы",
    "Акула",
    "Рюкзак Санта-Клауса",
    "Новогодний дракон",
    "Маска зелёный дракон"
};
new const g_fvc_acs_c[][] = {
    "Рюкзак LV",
    "Рюкзак Puma",
    "Рюкзак Nike",
    "Поясная сумка Gucci",
    "Сумка Supreme",
    "Рюкзак Gucci",
    "Сумка Gucci с павлином",
    "Кепка Gucci",
    "Кепка Россия",
    "Кепка Спартак",
    "Кепка ЦСКА",
    "Авиаторы фиолетовые",
    "Чёрные очки",
    "Apple Watch 5 чёрные",
    "Apple Watch 5 белые",
    "Часы Silver Casio",
    "Часы Black Casio",
    "Часы Gold Casio",
    "Акустическая гитара",
    "Электрогитара",
    "Скейт",
    "Скейтборд Tony Hawk",
    "Женская сумка",
    "Белая маска",
    "Чёрная маска",
    "Шапка Adidas",
    "Шапка Nike",
    "Шапка Calvin Klein",
    "Шапка Puma",
    "Шапка LV",
    "Шапка TNF",
    "Панама Gucci",
    "Панама Adidas",
    "Панама Nike",
    "Шарф Gucci",
    "Шарф",
    "Наушники-пингвин",
    "Наушники с ушками",
    "Красные наушники",
    "Кандибобер",
    "Берет",
    "Шапка-ушанка",
    "Шапка с помпоном",
    "Зимняя кепка",
    "Зимний рюкзак-олень",
    "Маска кошечки",
    "Маска тигра",
    "Маска курицы",
    "Маска голубя",
    "Маска медведя",
    "Самурайская маска",
    "Самурайский шлем",
    "Самурайская катана",
    "Самурайская шляпа",
    "Индейский головной убор",
    "Доска для серфинга",
    "Попугай",
    "Очки Крылья бабочки",
    "Очки Облака с грозой",
    "Очки Новогодние с рогами",
    "Очки-сердечки",
    "Очки-звёздочки",
    "Борода",
    "Сомбреро",
    "Велосипедный шлем",
    "Гребешок с рогами",
    "Рюкзак Котик",
    "Рюкзак Олаф",
    "Рюкзак Щелкунчик",
    "Бандана",
    "Треуголка",
    "Фуражка",
    "Пилотка",
    "Шляпка",
    "Зонт-шляпа",
    "Маска для дайвинга",
    "Кожаный кейс",
    "Пляжная шляпа",
    "Пляжные очки",
    "Летний рюкзак",
    "Маска Крейта",
    "Шар звезда",
    "Вертолёт"
};

new const g_fvc_skin_rare[][] = {
    "Илон Маск",
    "Литвин",
    "Админ",
    "Поло",
    "Дед Мороз",
    "Космонавт",
    "Шейх Дубай",
    "Спецназовец",
    "Богатырь",
    "Киберсамурай",
    "Мафиози",
    "Бизнесмен в костюме",
    "Тигр"
};
new const g_fvc_skin_common[][] = {
    "Гопник",
    "Мажор",
    "Спортсмен",
    "Рэпер",
    "Охранник",
    "Бандит",
    "Полицейский",
    "Ковбой",
    "Казак",
    "Шахтёр",
    "Дальнобойщик",
    "Лыжник",
    "Байкер",
    "Рыбак",
    "Бомж",
    "Клоун",
    "Таксист",
    "Строитель",
    "Курьер",
    "Фермер",
    "Охотник",
    "Пират",
    "Футболист",
    "Хипстер",
    "Школьник",
    "Адидас-пацан",
    "Кавказец",
    "Грузчик",
    "Врач",
    "Пожарный"
};

stock FVC_Rand(min, max)
{
    return min + random(max - min + 1);
}

// округляем до 2 значащих цифр, чтобы цены выглядели как у людей: 9,5кк, 150кк, 230к
stock FVC_RoundPrice(value)
{
    new step = 1;
    while(step <= value / 100) step *= 10;
    return (value + step / 2) / step * step;
}

stock FVC_FormatMoney(value, dest[], len)
{
    value = FVC_RoundPrice(value);
    new t;
    if(value >= 1000000000)
    {
        t = value / 100000000;
        if(t % 10) format(dest, len, "%d,%dккк", t / 10, t % 10);
        else format(dest, len, "%dккк", t / 10);
    }
    else if(value >= 1000000)
    {
        t = value / 100000;
        if(t % 10) format(dest, len, "%d,%dкк", t / 10, t % 10);
        else format(dest, len, "%dкк", t / 10);
    }
    else format(dest, len, "%dк", value / 1000);
    return 1;
}

// свободный (не занятый реальным игроком) ID в пределах maxplayers сервера
stock FVC_GetFakeId()
{
    new maxp = GetMaxPlayers(), id = 0;
    if(maxp < 1) maxp = 1;
    for(new tries = 0; tries < 25; tries++)
    {
        id = random(maxp);
        if(!IsPlayerConnected(id)) return id;
    }
    return id;
}

stock FVC_RandomPrefix(dest[], len)
{
    new r = random(100);
    if(r < 55) format(dest, len, "[VIP Silver]");
    else if(r < 88) format(dest, len, "[VIP Gold]");
    else format(dest, len, "[VIP Platinum]");
    return 1;
}

// 50/50: 4-значный или 6-значный номер
stock FVC_RandomPhone(dest[], len)
{
    if(random(2)) format(dest, len, "%d", FVC_Rand(1000, 9999));
    else format(dest, len, "%d", FVC_Rand(100000, 999999));
    return 1;
}

// ---- генераторы текста объявления -----------------------------------------
stock FVC_MakeCarAd(bool:buy, dest[], len)
{
    new c = random(sizeof g_fvc_cars), base = g_fvc_cars[c][FVC_CAR_PRICE], price, pm[24];
    if(buy) price = base / 100 * FVC_Rand(85, 105);
    else    price = base / 100 * FVC_Rand(100, 125);
    FVC_FormatMoney(price, pm, sizeof pm);

    if(buy)
    {
        switch(random(5))
        {
            case 0: format(dest, len, "Куплю т/с %s, бюджет до %s", g_fvc_cars[c][FVC_CAR_NAME], pm);
            case 1: format(dest, len, "Куплю т/с %s за %s", g_fvc_cars[c][FVC_CAR_NAME], pm);
            case 2: format(dest, len, "Куплю %s, до %s, сразу деньги", g_fvc_cars[c][FVC_CAR_NAME], pm);
            case 3: format(dest, len, "Куплю т/с %s - %s, торг", g_fvc_cars[c][FVC_CAR_NAME], pm);
            default: format(dest, len, "Куплю %s за %s", g_fvc_cars[c][FVC_CAR_NAME], pm);
        }
    }
    else
    {
        switch(random(4))
        {
            case 0: format(dest, len, "Продам т/с %s за %s", g_fvc_cars[c][FVC_CAR_NAME], pm);
            case 1: format(dest, len, "Продам т/с %s, цена %s, торг", g_fvc_cars[c][FVC_CAR_NAME], pm);
            case 2: format(dest, len, "Продам %s - %s, срочно", g_fvc_cars[c][FVC_CAR_NAME], pm);
            default: format(dest, len, "Продам %s за %s", g_fvc_cars[c][FVC_CAR_NAME], pm);
        }
    }
    return 1;
}

stock FVC_MakeHouseAd(dest[], len)
{
    new h = random(sizeof g_fvc_house_place), pm[24];
    FVC_FormatMoney(FVC_Rand(g_fvc_house_min[h], g_fvc_house_max[h]), pm, sizeof pm);
    switch(random(4))
    {
        case 0: format(dest, len, "Куплю дом %s, бюджет до %s", g_fvc_house_place[h], pm);
        case 1: format(dest, len, "Куплю дом %s за %s", g_fvc_house_place[h], pm);
        case 2: format(dest, len, "Куплю дом %s, до %s", g_fvc_house_place[h], pm);
        default: format(dest, len, "Куплю дом %s, цена %s, торг", g_fvc_house_place[h], pm);
    }
    return 1;
}

stock FVC_MakeSkinAd(bool:buy, dest[], len)
{
    new pm[24], name[24];
    if(random(100) < 40)
    {
        strmid(name, g_fvc_skin_rare[random(sizeof g_fvc_skin_rare)], 0, 23, sizeof name);
        FVC_FormatMoney(FVC_Rand(20000000, 150000000), pm, sizeof pm);
    }
    else
    {
        strmid(name, g_fvc_skin_common[random(sizeof g_fvc_skin_common)], 0, 23, sizeof name);
        FVC_FormatMoney(FVC_Rand(500000, 15000000), pm, sizeof pm);
    }
    if(buy)
    {
        switch(random(3))
        {
            case 0: format(dest, len, "Куплю о/п %s за %s", name, pm);
            case 1: format(dest, len, "Куплю о/п %s, до %s", name, pm);
            default: format(dest, len, "Куплю о/п (скин) %s - %s", name, pm);
        }
    }
    else
    {
        switch(random(3))
        {
            case 0: format(dest, len, "Продам о/п %s за %s", name, pm);
            case 1: format(dest, len, "Продам о/п %s, цена %s, торг", name, pm);
            default: format(dest, len, "Продам о/п (скин) %s - %s", name, pm);
        }
    }
    return 1;
}

stock FVC_MakeAcsAd(bool:buy, dest[], len)
{
    new pm[24], name[40], r = random(100);
    if(r < 12) // самые дорогие: 1 - 1,5ккк
    {
        strmid(name, g_fvc_acs_a[random(sizeof g_fvc_acs_a)], 0, 39, sizeof name);
        FVC_FormatMoney(FVC_Rand(1000000000, 1500000000), pm, sizeof pm);
    }
    else if(r < 40) // редкие рядом с ними: 100 - 200кк
    {
        strmid(name, g_fvc_acs_b[random(sizeof g_fvc_acs_b)], 0, 39, sizeof name);
        FVC_FormatMoney(FVC_Rand(100000000, 200000000), pm, sizeof pm);
    }
    else // обычные из магазина: 100 - 400к
    {
        strmid(name, g_fvc_acs_c[random(sizeof g_fvc_acs_c)], 0, 39, sizeof name);
        FVC_FormatMoney(FVC_Rand(100000, 400000), pm, sizeof pm);
    }
    if(buy)
    {
        switch(random(3))
        {
            case 0: format(dest, len, "Куплю а/с %s за %s", name, pm);
            case 1: format(dest, len, "Куплю а/с %s, до %s", name, pm);
            default: format(dest, len, "Куплю а/с %s - %s, торг", name, pm);
        }
    }
    else
    {
        switch(random(3))
        {
            case 0: format(dest, len, "Продам а/с %s за %s", name, pm);
            case 1: format(dest, len, "Продам а/с %s, цена %s", name, pm);
            default: format(dest, len, "Продам а/с %s - %s, торг", name, pm);
        }
    }
    return 1;
}

// ---- таймер ---------------------------------------------------------------
forward FVC_Tick();
public FVC_Tick()
{
    // следующий тик планируем сразу, чтобы цепочка не прерывалась
    SetTimer("FVC_Tick", FVC_DELAY_MIN + random(FVC_DELAY_JITTER + 1), false);

    if(!g_fvc_enabled) return 1;

    // есть ли вообще кому показывать (без аллокаций и форматирования, если нет)
    new receivers = 0;
    for(new i = 0; i < MAX_PLAYERS; i++)
    {
        if(!IsPlayerConnected(i) || IsPlayerNPC(i)) continue;
        if(!IsPlayerLogged(i)) continue;
        if(!GetPlayerPremium(i)) continue;
        if(!GetPlayerData(i, P_PREMIUM_CHAT)) continue;
        receivers++;
    }
    if(!receivers) return 1;

    new ad[96], prefix[16], phone[8], nick[32], msg[144], r = random(100);

    // доли: куплю т/с 36%, продам т/с 22%, дом 12%, о/п 14%, а/с 16%
    if(r < 36)       FVC_MakeCarAd(true,  ad, sizeof ad);
    else if(r < 58)  FVC_MakeCarAd(false, ad, sizeof ad);
    else if(r < 70)  FVC_MakeHouseAd(ad, sizeof ad);
    else if(r < 78)  FVC_MakeSkinAd(true,  ad, sizeof ad);
    else if(r < 84)  FVC_MakeSkinAd(false, ad, sizeof ad);
    else if(r < 93)  FVC_MakeAcsAd(true,  ad, sizeof ad);
    else             FVC_MakeAcsAd(false, ad, sizeof ad);

    FVC_RandomPrefix(prefix, sizeof prefix);
    FVC_RandomPhone(phone, sizeof phone);
    format(nick, sizeof nick, "%s_%s", g_fvc_first[random(sizeof g_fvc_first)], g_fvc_last[random(sizeof g_fvc_last)]);

    format(msg, sizeof msg, "%s %s[%d]: %s Тел: %s", prefix, nick, FVC_GetFakeId(), ad, phone);

    for(new i = 0; i < MAX_PLAYERS; i++)
    {
        if(!IsPlayerConnected(i) || IsPlayerNPC(i)) continue;
        if(!IsPlayerLogged(i)) continue;
        if(!GetPlayerPremium(i)) continue;
        if(!GetPlayerData(i, P_PREMIUM_CHAT)) continue;
        SendClientMessage(i, FVC_COLOR, msg);
    }
    return 1;
}

CMD:fakevip(playerid, params[])
{
    #pragma unused params
    if(!IsPlayerAdmin(playerid)) return 0;
    g_fvc_enabled = !g_fvc_enabled;
    SendClientMessage(playerid, -1, g_fvc_enabled ? ("Фейковые объявления в VIP-чате: {66CC33}включены") : ("Фейковые объявления в VIP-чате: {FF6347}выключены"));
    return 1;
}
// ============================================================================
