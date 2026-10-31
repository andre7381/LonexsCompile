
// ============================================================================
// FAKE VIP CHAT v2 (чисто визуальная имитация живого /v)
// - никаких ботов в игре: ни слотов, ни NPC, ни записей в БД, ни MySQL-запросов
// - одно сообщение раз в FVC_DELAY_MIN..FVC_DELAY_MIN+FVC_DELAY_JITTER мс (5-10 сек)
// - объявления (куплю/продам т/с, дом, о/п, а/с) + болтовня, приветы и мат
// - на очень дорогие цены (от FVC_OVER_PRICE) "игроки" отвечают "овер" и т.п.
// - ник, ID, префикс и номер телефона каждый раз случайные
// - рассылается только онлайн-игрокам с включённым VIP-чатом (как обычный /v)
// - по умолчанию ВЫКЛЮЧЕНО. Включить/выключить: /fakevip (или /fakevip on, /fakevip off)
// ============================================================================
#define FVC_DELAY_MIN       5000    // минимальная пауза между сообщениями, мс
#define FVC_DELAY_JITTER    5000    // + случайная добавка 0..N мс (итого 5-10 сек)
#define FVC_OVER_PRICE      400000000   // от такой цены "игроки" пишут "овер"
#define FVC_COLOR           0xE8F808FF
#define FVC_ADMIN_LEVEL     1       // с какого уровня админки доступна /fakevip (RCON-админ тоже может)

enum E_FVC_CAR { FVC_CAR_NAME[40], FVC_CAR_RU[48], FVC_CAR_PRICE };

new bool:g_fvc_enabled = false;   // по умолчанию ВЫКЛЮЧЕНО, включается командой /fakevip
new g_fvc_last_price;       // цена последнего сгенерированного объявления
new g_fvc_react;            // 1 = следующим сообщением реакция "овер"
new g_fvc_react2;           // 1 = после неё ещё одна реплика "согл/лол"
new g_fvc_greet;            // 1 = следующим сообщением ответный привет

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

// авто взяты из g_vehicle_info: латиница, как называют в чате, и гос. цена
new const g_fvc_cars[][E_FVC_CAR] = {
    {"VAZ 2108", "ваз 2108", 170000},
    {"VAZ 2109", "ваз 2109", 180000},
    {"VAZ 2110", "ваз 2110", 190000},
    {"VAZ 2114", "ваз 2114", 195000},
    {"VAZ 2115", "ваз 2115", 210000},
    {"VAZ 2172", "ваз 2172", 300000},
    {"BMW M3 E30", "бмв м3 е30", 300000},
    {"Niva Urban", "нива урбан", 350000},
    {"Lada Granta", "гранта", 370000},
    {"VAZ 2112", "ваз 2112", 420000},
    {"Toyota Mark II", "тойота марк 2", 420000},
    {"Nissan Skyline R34", "ниссан скайлайн р34", 680000},
    {"Mercedes-Benz E420 W210", "мерс е420 в210", 700000},
    {"Lada Vesta", "лада веста", 700000},
    {"Renault Logan", "рено логан", 716000},
    {"Audi A4", "ауди а4", 750000},
    {"Lada Vesta SW", "лада веста св", 764000},
    {"BMW M5 E39", "бмв м5 е39", 800000},
    {"Kia Rio", "киа рио", 815000},
    {"Hyundai Solaris 2021", "хендай солярис", 830000},
    {"Volkswagen Polo", "поло", 850000},
    {"Skoda Rapid", "шкода рапид", 870000},
    {"Mercedes-Benz S600 W140", "мерс с600 в140", 900000},
    {"BMW E38 750i", "бмв е38 750и", 950000},
    {"BMW M3 E46", "бмв м3 е46", 1050000},
    {"Acura TSX", "акура тсх", 1150000},
    {"Uaz Patriot", "уаз патриот", 1200000},
    {"BMW M3 E92", "бмв м3 е92", 1250000},
    {"BMW E38 L7", "бмв е38 л7", 1360000},
    {"Volkswagen Golf GTI", "фольксваген гольф гти", 1400000},
    {"ИЖ Юпитер", "иж юпитер", 1500000},
    {"BMW X5 E53", "бмв х5 е53", 1500000},
    {"Ford Focus RS", "форд фокус рс", 1800000},
    {"Ducati SuperSport S", "дукати суперспорт с", 1830000},
    {"Mitsubishi Lancer Evo X", "митсубиси лансер эво икс", 2000000},
    {"Toyota Camry 2016", "камри", 2000000},
    {"BMW M5 E60", "бмв м5 е60", 2100000},
    {"Subaru WRX STI", "субару врх сти", 2200000},
    {"Toyota Camry 3.5", "камри 3.5", 2400000},
    {"Kia K5", "киа к5", 2550000},
    {"Ford Mustang GT", "форд мустанг гт", 2600000},
    {"Toyota Supra A80", "тойота супра а80", 2700000},
    {"Mercedes-Benz A45 AMG", "мерс а45 амг", 2800000},
    {"Ducati XDiavel S", "дукати ходиавел с", 3000000},
    {"Alfa Romeo Giulia", "альфа ромео джулия", 3000000},
    {"Infiniti Q60S", "инфинити ку60с", 3200000},
    {"Volvo V60", "вольво в60", 3500000},
    {"Volkswagen Touareg R-Line", "фольксваген туарег р лайн", 3900000},
    {"Volkswagen Multivan T6", "фольксваген мультивэн т6", 4000000},
    {"Honda NSX-R NA2", "хонда нсх р на2", 4140000},
    {"Volvo XC90", "вольво хс90", 4200000},
    {"Toyota GT-86", "тойота гт86", 4200000},
    {"KIA Stinger", "киа стингер", 4200000},
    {"Jeep Grand Cherokee Trackhawk SRT", "джип гранд чероки трэкхок срт", 4250000},
    {"Yamaha FZ-10", "ямаха фз10", 4500000},
    {"Infiniti Q50", "инфинити ку50", 4500000},
    {"Chevrolet Camaro ZL1", "шевроле камаро зл1", 4600000},
    {"BMW 3-Series G20", "бмв 3 серии г20", 4700000},
    {"Lincoln Continental", "линкольн континенталь", 4800000},
    {"BMW Z4 M40i", "бмв з4 м40и", 4900000},
    {"Yamaha YZF-R6", "ямаха юзф р6", 5000000},
    {"Bentley Bentayga", "бентли бентайга", 5200000},
    {"Jeep Wrangler JL", "джип вранглер джл", 5300000},
    {"BMW M4 F84", "бмв м4 ф84", 5500000},
    {"Ford Raptor F-150", "форд раптор ф150", 5757000},
    {"Toyota Supra A90", "тойота супра а90", 5900000},
    {"Dodge Demon SRT", "додж демон срт", 6100000},
    {"BMW M2 CS", "бмв м2 сс", 6170000},
    {"Mercedes-Benz V-Class W447", "мерс в класс в447", 6300000},
    {"Ferrari 612 Scaglietti", "феррари 612 скальетти", 6680000},
    {"Mercedes-Benz GT63s", "мерс гт63с", 7000000},
    {"Audi Q7", "ауди ку7", 7000000},
    {"BMW S 1000 RR", "бмв с 1000 рр", 7000000},
    {"Audi Q7 2020", "ауди ку7", 7175000},
    {"Cadillac Escalade", "кадиллак эскалейд", 7200000},
    {"Dodge Charger V2", "додж чарджер в2", 7250000},
    {"Jaguar F-Pace", "ягуар ф пейс", 7450000},
    {"BMW M5 F10", "бмв м5 ф10", 7700000},
    {"Toyota Land Cruiser 200", "тойота ленд крузак 200", 7800000},
    {"Nissan GT-R R35", "ниссан гтр р35", 7900000},
    {"Suzuki GSX-R750", "сузуки гсх р750", 8000000},
    {"BMW X6M F16", "бмв х6м ф16", 8200000},
    {"Porsche Panamera S", "порш панамера с", 8400000},
    {"BMW X6 M F96", "бмв х6 м ф96", 8450000},
    {"Audi RS6", "ауди рс6", 8500000},
    {"Honda NSX 2017", "хонда нсх", 8600000},
    {"Mercedes-Benz C63 AMG", "мерс с63 амг", 8600000},
    {"BMW M6 F12", "бмв м6 ф12", 8700000},
    {"Chevrolet Tahoe RST", "шевроле тахо рст", 8900000},
    {"Lexus LX500", "лексус лх500", 9000000},
    {"Porsche 911 Carrera S", "порш 911 каррера с", 9000000},
    {"Mercedes-Benz GLS 400d", "мерс глс 400д", 9150000},
    {"BMW M5 F90", "бмв м5 ф90", 9500000},
    {"Audi RS7 Sport", "ауди рс7 спорт", 9500000},
    {"Audi RS7 C8", "ауди рс7 с8", 9650000},
    {"Jaguar F-Type", "ягуар ф тайп", 9800000},
    {"Audi RS6 C7", "ауди рс6 с7", 10000000},
    {"Range Rover SVR", "рендж ровер свр", 10000000},
    {"Kawasaki Ninja H2R", "кавасаки ниндзя аш2р", 10000000},
    {"BMW M5 F90 2021", "бмв м5 ф90", 10250000},
    {"Mercedes-Benz C63s", "мерс с63с", 10300000},
    {"Cadillac Escalade SUV", "кадиллак эскалейд", 10370000},
    {"BMW X7 M50i", "бмв х7 м50и", 10500000},
    {"Audi R8 V10", "ауди р8 в10", 11000000},
    {"Mercedes-Benz CLS63 AMG", "мерс цлс63 амг", 11000000},
    {"Mercedes-Benz E63s", "мерс е63с", 11450000},
    {"Lexus LX 570", "лексус лх 570", 11456000},
    {"Cadillac Escalade 2021", "кадиллак эскалейд", 11470000},
    {"Range Rover Evoque", "рендж ровер эвок", 11500000},
    {"BMW I8 EDrive", "бмв ай8 эдрайв", 12600000},
    {"BMW I8 Roadster", "бмв ай8 роудстер", 12600000},
    {"BMW X5M G05", "бмв х5м г05", 13000000},
    {"Mercedes-Benz SL63 2023", "мерс сл63", 13450000},
    {"Mercedes-Benz GT-R", "мерс гтр", 13500000},
    {"McLaren 600LT", "макларен 600лт", 14000000},
    {"BMW 7-Series 750Li", "бмв 7 серии 750ли", 14300000},
    {"Porsche Cayenne S", "порш кайен с", 14700000},
    {"Audi RS6 C8", "ауди рс6 с8", 14800000},
    {"Range Rover Velar", "рендж ровер велар", 15000000},
    {"Lamborghini Urus", "ламба урус", 15300000},
    {"Tesla Model S", "тесла модел с", 15500000},
    {"BMW M760Li xDrive", "бмв м760ли хдрайв", 16400000},
    {"Lamborghini Huracan", "ламба хуракан", 16500000},
    {"Chevrolet Corvette C8", "шевроле корвет с8", 17000000},
    {"Audi A8 2022", "ауди а8", 17000000},
    {"Mercedes-Benz G65 AMG", "гелик г65", 17050000},
    {"BMW M8 F92", "бмв м8 ф92", 17200000},
    {"Lamborghini Urus Venum", "ламба урус венум", 17350000},
    {"Ferrari 488 GTB", "феррари 488 гтб", 17500000},
    {"Porsche Taycan Turbo S", "порш тайкан турбо с", 18300000},
    {"Aston Martin DB11", "астон мартин дб11", 18500000},
    {"BMW M5 Competition Asco", "бмв м5 компетишн аско", 19000000},
    {"BMW M8 F93", "бмв м8 ф93", 19000000},
    {"BMW M8 F93 Gran Coupe", "бмв м8 ф93 гран купе", 19000000},
    {"Mercedes-Benz 300SL Coupe", "мерс 300сл купе", 19500000},
    {"Lamborghini Aventador S", "ламба авентадор с", 20000000},
    {"Tesla Model X", "тесла модел икс", 20500000},
    {"Mercedes-Benz G63 AMG", "гелик г63", 23500000},
    {"Mercedes-Benz EQS580", "мерс экью эс 580", 24000000},
    {"Mercedes-Benz Maybach S650", "мерс майбах с650", 25000000},
    {"Ferrari 488 Pista", "феррари 488 писта", 26000000},
    {"Rolls-Royce Wraith", "ролс ройс рейф", 30000000},
    {"Aurus Senat", "аурус сенат", 30000000},
    {"Rolls-Royce Cullinan", "ролс ройс куллинан", 40000000},
    {"Rolls-Royce Phantom", "ролс ройс фантом", 50000000},
    {"Bentley Continental GT", "бентли континенталь гт", 54000000},
    {"BMW M1", "бмв м1", 70000000},
    {"Bugatti Atlantic", "бугатти атлантик", 75000000},
    {"Bentley Mulliner Becalar", "бентли малинер бекалар", 125000000},
    {"Bugatti Chiron", "бугатти шерон", 140000000},
    {"Bugatti Divo", "бугатти диво", 380000000},
    {"Bugatti La Noire", "бугатти ла нуар", 1000000000}
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

// болтовня
new const g_fvc_greet_list[][] = {
    "привет",
    "всем привет",
    "ку",
    "здарова",
    "всем ку",
    "хай",
    "доброго времени суток",
    "салам",
    "здравствуйте",
    "добрый вечер",
    "всем прив",
    "здорова народ",
    "приветики",
    "добрый день",
    "всем доброй ночи",
    "утро доброе",
    "здрасьте всем"
};
new const g_fvc_greet_reply[][] = {
    "привет",
    "ку",
    "здарова",
    "салам",
    "хай",
    "привет))",
    "о, привет",
    "здорова",
    "привет всем",
    "ку ку",
    "прив",
    "здарова бро"
};
new const g_fvc_chat_list[][] = {
    "как дела у всех?",
    "че как",
    "кто тут давно играет?",
    "скучно что-то",
    "го в казино",
    "кто в семью берёт?",
    "кто на рублёвке живёт?",
    "норм сервер сегодня",
    "лагает чето",
    "кто дальнобой ездит?",
    "кто знает где сейчас продают машины?",
    "всем хорошего дня",
    "вип чат живой вообще?",
    "кто хочет подработку?",
    "ребят а где можно скины купить?",
    "кто сегодня на капт?",
    "опять ресет какой-то",
    "ну и лаги бля",
    "пиздец цены на дома",
    "охуеть цены сейчас",
    "дохуя всего подорожало",
    "сука опять кикнуло",
    "бля кто знает где фармить?",
    "нахуй такие цены в магазине",
    "вот это тачки пошли",
    "ахаха",
    "лол",
    "ржу",
    "да ну нахуй",
    "ебать тут жизнь кипит",
    "чето тихо в чате",
    "кто на связи?",
    "всем удачи",
    "ладно пошёл фармить",
    "надо бы бизнес купить",
    "кто знает где дешевле всего дом?",
    "у кого есть бизнес? сколько приносит?",
    "я вам говорю цены вырастут",
    "вип лучшая вещь тут",
    "дайте денег ахах",
    "пойду на работу",
    "что по ценам на тачки сегодня?",
    "кто в нижегородске сейчас?",
    "на бусаево кто нить есть?",
    "кому нужна помощь с фармом пишите",
    "кто хочет в банду?",
    "опять эти лаги, достало",
    "бля я только что разбил тачку",
    "кто видел новые аксы в магазине?",
    "а чо дорого так всё стало",
    "пиздец инфляция какая-то",
    "хуйня а не сервер, лагает постоянно",
    "норм всё, играем",
    "я за вип отдал не зря",
    "кто на чём ездит?",
    "вот бы сейчас на рублёвку дом",
    "мне бы тачку покруче",
    "кто знает когда обнова?",
    "пацаны го на заправку",
    "блин забыл заправиться и встал на трассе",
    "так, кому тачку дёшево?",
    "ща бы в кайф дом на рублёвке",
    "чат вип самый живой",
    "кто тут за мир во всём мире"
};
// реакции на дорогие цены
new const g_fvc_over_list[][] = {
    "овер",
    "овер цена",
    "овер))",
    "ахах овер",
    "дохуя просишь",
    "охуел?",
    "ты че",
    "лол овер",
    "хуя цена",
    "дорого пиздец",
    "слишком дорого",
    "ну такое",
    "овер цена бля",
    "нахуй такие цены",
    "ты охуел с такой ценой",
    "ахаха оверпрайс",
    "ни хуя себе",
    "это овер",
    "не, овер",
    "дохуя",
    "ахаха овер",
    "цена мечты",
    "не продашь за такое",
    "да за такие деньги я лучше дом куплю",
    "ты с ума сошёл?"
};
new const g_fvc_agree_list[][] = {
    "согл",
    "да реально",
    "ахаха",
    "+",
    "лол",
    "+1",
    "это да",
    "ржач",
    "жиза",
    "вот именно"
};
new const g_fvc_phone_label[][] = {
    "Тел: ",
    "тел ",
    "номер ",
    "тел. ",
    "звонить ",
    "писать на ",
    "номер: ",
    "т. ",
    "тел:",
    "пишите "
};

stock FVC_Rand(min, max)
{
    return min + random(max - min + 1);
}

// всё в нижний регистр (латиница и кириллица cp1251)
stock FVC_LowerAll(str[])
{
    for(new i = 0, c; str[i]; i++)
    {
        c = str[i];
        if((c >= 'A' && c <= 'Z') || (c >= 192 && c <= 223)) str[i] = c + 32;
        else if(c == 168) str[i] = 184; // Ё -> ё
    }
    return 1;
}

stock FVC_LowerFirst(str[])
{
    new c = str[0];
    if((c >= 'A' && c <= 'Z') || (c >= 192 && c <= 223)) str[0] = c + 32;
    return 1;
}

// как будто человек писал руками: чаще всё с маленькой буквы
stock FVC_Humanize(str[])
{
    new r = random(100);
    if(r < 55) FVC_LowerAll(str);
    else if(r < 85) FVC_LowerFirst(str);
    return 1;
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
    g_fvc_last_price = value;
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

// чаще пишут по-русски ("бмв м5 ф90"), иногда латиницей
stock FVC_GetCarName(c, dest[], len)
{
    if(random(100) < 70) format(dest, len, "%s", g_fvc_cars[c][FVC_CAR_RU]);
    else format(dest, len, "%s", g_fvc_cars[c][FVC_CAR_NAME]);
    return 1;
}

// ---- генераторы текста объявления -----------------------------------------
stock FVC_MakeCarAd(bool:buy, dest[], len)
{
    new c = random(sizeof g_fvc_cars), base = g_fvc_cars[c][FVC_CAR_PRICE], price, pm[24], car[48];
    FVC_GetCarName(c, car, sizeof car);
    if(buy) price = base / 100 * FVC_Rand(85, 105);
    else    price = base / 100 * FVC_Rand(100, 125);
    FVC_FormatMoney(price, pm, sizeof pm);

    if(buy)
    {
        switch(random(7))
        {
            case 0: format(dest, len, "Куплю т/с %s, бюджет до %s", car, pm);
            case 1: format(dest, len, "Куплю т/с %s за %s", car, pm);
            case 2: format(dest, len, "Куплю %s, до %s, сразу деньги", car, pm);
            case 3: format(dest, len, "Куплю т/с %s - %s, торг", car, pm);
            case 4: format(dest, len, "Куплю тс %s до %s", car, pm);
            case 5: format(dest, len, "Куплю %s %s торг", car, pm);
            default: format(dest, len, "Куплю %s за %s", car, pm);
        }
    }
    else
    {
        switch(random(5))
        {
            case 0: format(dest, len, "Продам т/с %s за %s", car, pm);
            case 1: format(dest, len, "Продам т/с %s, цена %s, торг", car, pm);
            case 2: format(dest, len, "Продам %s - %s, срочно", car, pm);
            case 3: format(dest, len, "Продам тс %s %s", car, pm);
            default: format(dest, len, "Продам %s за %s", car, pm);
        }
    }
    return 1;
}

stock FVC_MakeHouseAd(dest[], len)
{
    new h = random(sizeof g_fvc_house_place), pm[24];
    FVC_FormatMoney(FVC_Rand(g_fvc_house_min[h], g_fvc_house_max[h]), pm, sizeof pm);
    switch(random(5))
    {
        case 0: format(dest, len, "Куплю дом %s, бюджет до %s", g_fvc_house_place[h], pm);
        case 1: format(dest, len, "Куплю дом %s за %s", g_fvc_house_place[h], pm);
        case 2: format(dest, len, "Куплю дом %s, до %s", g_fvc_house_place[h], pm);
        case 3: format(dest, len, "Куплю дом %s %s", g_fvc_house_place[h], pm);
        default: format(dest, len, "Куплю дом %s, цена %s, торг", g_fvc_house_place[h], pm);
    }
    return 1;
}

stock FVC_MakeSkinAd(bool:buy, dest[], len)
{
    new pm[24], name[24];
    if(random(100) < 40)
    {
        format(name, sizeof name, "%s", g_fvc_skin_rare[random(sizeof g_fvc_skin_rare)]);
        FVC_FormatMoney(FVC_Rand(20000000, 150000000), pm, sizeof pm);
    }
    else
    {
        format(name, sizeof name, "%s", g_fvc_skin_common[random(sizeof g_fvc_skin_common)]);
        FVC_FormatMoney(FVC_Rand(500000, 15000000), pm, sizeof pm);
    }
    if(buy)
    {
        switch(random(4))
        {
            case 0: format(dest, len, "Куплю о/п %s за %s", name, pm);
            case 1: format(dest, len, "Куплю о/п %s, до %s", name, pm);
            case 2: format(dest, len, "Куплю оп %s %s", name, pm);
            default: format(dest, len, "Куплю о/п (скин) %s - %s", name, pm);
        }
    }
    else
    {
        switch(random(4))
        {
            case 0: format(dest, len, "Продам о/п %s за %s", name, pm);
            case 1: format(dest, len, "Продам о/п %s, цена %s, торг", name, pm);
            case 2: format(dest, len, "Продам оп %s %s", name, pm);
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
        format(name, sizeof name, "%s", g_fvc_acs_a[random(sizeof g_fvc_acs_a)]);
        FVC_FormatMoney(FVC_Rand(1000000000, 1500000000), pm, sizeof pm);
    }
    else if(r < 40) // редкие рядом с ними: 100 - 200кк
    {
        format(name, sizeof name, "%s", g_fvc_acs_b[random(sizeof g_fvc_acs_b)]);
        FVC_FormatMoney(FVC_Rand(100000000, 200000000), pm, sizeof pm);
    }
    else // обычные из магазина: 100 - 400к
    {
        format(name, sizeof name, "%s", g_fvc_acs_c[random(sizeof g_fvc_acs_c)]);
        FVC_FormatMoney(FVC_Rand(100000, 400000), pm, sizeof pm);
    }
    if(buy)
    {
        switch(random(4))
        {
            case 0: format(dest, len, "Куплю а/с %s за %s", name, pm);
            case 1: format(dest, len, "Куплю а/с %s, до %s", name, pm);
            case 2: format(dest, len, "Куплю ас %s %s", name, pm);
            default: format(dest, len, "Куплю а/с %s - %s, торг", name, pm);
        }
    }
    else
    {
        switch(random(4))
        {
            case 0: format(dest, len, "Продам а/с %s за %s", name, pm);
            case 1: format(dest, len, "Продам а/с %s, цена %s", name, pm);
            case 2: format(dest, len, "Продам ас %s %s", name, pm);
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

    // есть ли вообще кому показывать (без форматирования, если нет)
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

    new text[100], prefix[16], phone[8], nick[32], msg[144], bool:is_ad = false, r;
    text[0] = 0;

    // 1) реакции на предыдущее сообщение
    if(g_fvc_react)
    {
        g_fvc_react = 0;
        if(random(100) < 85) format(text, sizeof text, "%s", g_fvc_over_list[random(sizeof g_fvc_over_list)]);
        else g_fvc_react2 = 0;
    }
    else if(g_fvc_react2)
    {
        g_fvc_react2 = 0;
        if(random(100) < 70) format(text, sizeof text, "%s", g_fvc_agree_list[random(sizeof g_fvc_agree_list)]);
    }
    else if(g_fvc_greet)
    {
        g_fvc_greet = 0;
        if(random(100) < 65) format(text, sizeof text, "%s", g_fvc_greet_reply[random(sizeof g_fvc_greet_reply)]);
    }

    // 2) новое сообщение: ~63% объявления, ~12% приветы, ~25% болтовня/мат
    if(!text[0])
    {
        r = random(100);
        if(r < 63)
        {
            is_ad = true;
            g_fvc_last_price = 0;
            r = random(63);
            if(r < 24)      FVC_MakeCarAd(true,  text, sizeof text);   // куплю т/с - чаще всего
            else if(r < 38) FVC_MakeCarAd(false, text, sizeof text);   // продам т/с
            else if(r < 45) FVC_MakeHouseAd(text, sizeof text);
            else if(r < 50) FVC_MakeSkinAd(true,  text, sizeof text);
            else if(r < 53) FVC_MakeSkinAd(false, text, sizeof text);
            else if(r < 59) FVC_MakeAcsAd(true,  text, sizeof text);
            else            FVC_MakeAcsAd(false, text, sizeof text);
            FVC_Humanize(text);

            // дорогая цена -> кто-то напишет "овер"
            if(g_fvc_last_price >= FVC_OVER_PRICE && random(100) < 80)
            {
                g_fvc_react = 1;
                g_fvc_react2 = (random(100) < 35) ? 1 : 0;
            }
        }
        else if(r < 75)
        {
            format(text, sizeof text, "%s", g_fvc_greet_list[random(sizeof g_fvc_greet_list)]);
            g_fvc_greet = 1;
        }
        else format(text, sizeof text, "%s", g_fvc_chat_list[random(sizeof g_fvc_chat_list)]);
    }

    FVC_RandomPrefix(prefix, sizeof prefix);
    format(nick, sizeof nick, "%s_%s", g_fvc_first[random(sizeof g_fvc_first)], g_fvc_last[random(sizeof g_fvc_last)]);

    if(is_ad)
    {
        FVC_RandomPhone(phone, sizeof phone);
        format(msg, sizeof msg, "%s %s[%d]: %s %s%s", prefix, nick, FVC_GetFakeId(), text, g_fvc_phone_label[random(sizeof g_fvc_phone_label)], phone);
    }
    else format(msg, sizeof msg, "%s %s[%d]: %s", prefix, nick, FVC_GetFakeId(), text);

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
    if(GetPlayerAdminEx(playerid) < FVC_ADMIN_LEVEL && !IsPlayerAdmin(playerid))
        return SendClientMessage(playerid, 0xCECECEFF, "Эта команда Вам недоступна.");

    if(!strcmp(params, "on", true)) g_fvc_enabled = true;
    else if(!strcmp(params, "off", true)) g_fvc_enabled = false;
    else g_fvc_enabled = !g_fvc_enabled;

    // сбрасываем цепочки реакций, чтобы после включения не вылезло "овер" из прошлого
    g_fvc_react = 0; g_fvc_react2 = 0; g_fvc_greet = 0;

    SendClientMessage(playerid, -1, g_fvc_enabled ? ("Фейковый VIP-чат: {66CC33}ВКЛЮЧЁН") : ("Фейковый VIP-чат: {FF6347}ВЫКЛЮЧЕН"));
    return 1;
}
// ============================================================================
