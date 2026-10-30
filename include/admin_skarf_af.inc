// Система форм на наказания /af.
// Команда доступна с 1 уровня администратора. Меню:
//   1) Подать форму   -> выбор наказания (Mute, Jail, Warn, Kick, Ban, Permban)
//                        -> окно формы: Ник, Срок, Причина (у Warn, Kick и Permban срока нет) -> отправка
//   2) Активные формы -> список -> просмотр -> принять / отклонить
// Принять форму может администратор, у которого есть доступ к выдаче этого наказания
// (уровни заданы в таблице g_SRAFType) и который не является автором формы.
// Причина наказания при принятии: "Причина | By N.Name" (N.Name - автор формы).
// Используются только два диалога (DIALOG_AF_LIST и DIALOG_AF_VIEW), шаг определяется PVar "SRAfStage",
// поэтому новых DIALOG_* в icerussia.pwn добавлять не нужно.
// В icerussia.pwn обработчик диалога должен получать inputtext:
//   return SRAdmin_AFDialogResponse(playerid, dialogid, response, listitem, inputtext);
#if defined _sr_admin_af_included
    #endinput
#endif
#define _sr_admin_af_included

#if !defined DIALOG_AF_LIST
    #define DIALOG_AF_LIST      (31901)
    #define DIALOG_AF_VIEW      (31902)
#endif

#define SR_AF_MIN_LEVEL         (1)         // /af доступен с 1 уровня
#define SR_AF_MAX_FORMS         (20)        // общий лимит форм в системе
#define SR_AF_MAX_PER_ADMIN     (3)         // активных форм от одного автора
#define SR_AF_LIFETIME          (1800)      // форма живёт 30 минут
#define SR_AF_REASON_LIMIT      (64)        // макс. длина итоговой причины (буфер команд [65])
#define SR_AF_COLOR_NEW         (0xFFAA00FF)
#define SR_AF_COLOR_PUNISH      (0xFF5533FF)

// Шаги меню (PVar "SRAfStage"). Диалог DIALOG_AF_LIST: 1-3, диалог DIALOG_AF_VIEW: 4-6.
#define SR_AF_STAGE_MENU        (1)         // главное меню
#define SR_AF_STAGE_TYPES       (2)         // выбор наказания
#define SR_AF_STAGE_LIST        (3)         // список активных форм
#define SR_AF_STAGE_FORM        (4)         // окно формы (Ник / Срок / Причина)
#define SR_AF_STAGE_INPUT       (5)         // ввод одного поля формы
#define SR_AF_STAGE_VIEW        (6)         // просмотр одной формы

// Строки окна формы.
#define SR_AF_ROW_NICK          (0)
#define SR_AF_ROW_TERM          (1)
#define SR_AF_ROW_REASON        (2)
#define SR_AF_ROW_SEND          (3)
#define SR_AF_ROW_BACK          (4)

// Строк с информацией в окне просмотра формы; дальше идут кнопки: принять, отклонить, назад.
#define SR_AF_VIEW_INFO_ROWS    (6)

#define SR_AF_PERMBAN_DAYS      (9999)      // как в /permban

// Типы наказаний.
#define SR_AF_MUTE              (0)
#define SR_AF_JAIL              (1)
#define SR_AF_WARN              (2)
#define SR_AF_KICK              (3)
#define SR_AF_BAN               (4)
#define SR_AF_PERMBAN           (5)
#define SR_AF_TYPES             (6)

// Name      - название в меню
// Level     - с какого уровня админ может ПРИНЯТЬ форму (как у самой команды)
// HasTerm   - есть ли в форме поле "Срок" (1 - есть)
// TermMin/TermMax - допустимый срок, Unit - единица срока
// NeedOnline- 1: форму можно подать и принять, только пока игрок в сети
enum E_SR_AF_TYPE
{
    SRAT_Name[10],
    SRAT_Level,
    SRAT_HasTerm,
    SRAT_TermMin,
    SRAT_TermMax,
    SRAT_NeedOnline,
    SRAT_Unit[8]
};
new const g_SRAFType[SR_AF_TYPES][E_SR_AF_TYPE] =
{
    {"Mute",    1, 1, 1, 360, 1, "мин."},
    {"Jail",    1, 1, 1, 360, 1, "мин."},
    {"Warn",    2, 0, 0, 0,   1, ""},
    {"Kick",    1, 0, 0, 0,   1, ""},
    {"Ban",     3, 1, 1, 30,  0, "дн."},
    {"Permban", 8, 0, 0, 0,   0, ""}
};

enum E_SR_AF_FORM
{
    SRAF_Serial,                            // 0 = слот свободен
    SRAF_Time,
    SRAF_Type,
    SRAF_CreatorAcc,
    SRAF_CreatorName[MAX_PLAYER_NAME + 1],
    SRAF_TargetAcc,
    SRAF_TargetName[MAX_PLAYER_NAME + 1],
    SRAF_Term,
    SRAF_Reason[SR_AF_REASON_LIMIT + 1]     // причина БЕЗ суффикса " | By N.Name"
};

new g_SRAFForm[SR_AF_MAX_FORMS][E_SR_AF_FORM];
new g_SRAFSerial;
new g_SRAFRow[MAX_PLAYERS][SR_AF_MAX_FORMS];

// ---------------------------------------------------------------------------------------------
// Вспомогательные функции
// ---------------------------------------------------------------------------------------------

// Сбрасывает только черновик формы (тип, ник, срок, причина).
stock SRAdmin_AFDraftClear(playerid)
{
    DeletePVar(playerid, "SRAfType");
    DeletePVar(playerid, "SRAfNick");
    DeletePVar(playerid, "SRAfTerm");
    DeletePVar(playerid, "SRAfReason");
    DeletePVar(playerid, "SRAfField");
    return 1;
}

stock SRAdmin_AFReset(playerid)
{
    DeletePVar(playerid, "SRAfStage");
    DeletePVar(playerid, "SRAfSerial");
    SRAdmin_AFDraftClear(playerid);
    for(new i; i < SR_AF_MAX_FORMS; i++) g_SRAFRow[playerid][i] = 0;
    return 1;
}

stock SRAdmin_AFError(playerid, const text[])
{
    SendClientMessage(playerid, -1, text);
    return 0;
}

stock SRAdmin_AFPurge()
{
    new now = gettime();
    for(new i; i < SR_AF_MAX_FORMS; i++)
        if(g_SRAFForm[i][SRAF_Serial] && now - g_SRAFForm[i][SRAF_Time] > SR_AF_LIFETIME)
            g_SRAFForm[i][SRAF_Serial] = 0;
    return 1;
}

stock SRAdmin_AFCount()
{
    new count;
    for(new i; i < SR_AF_MAX_FORMS; i++)
        if(g_SRAFForm[i][SRAF_Serial]) count++;
    return count;
}

stock SRAdmin_AFFind(serial)
{
    if(serial <= 0) return -1;
    for(new i; i < SR_AF_MAX_FORMS; i++)
        if(g_SRAFForm[i][SRAF_Serial] == serial) return i;
    return -1;
}

stock SRAdmin_AFFindOnline(account)
{
    foreach(new i : Player)
        if(IsPlayerLogged(i) && GetPlayerAccountID(i) == account) return i;
    return INVALID_PLAYER_ID;
}

// Vasya_Pupkin -> "V.Pupkin"
stock SRAdmin_AFAuthor(const name[], dest[], size)
{
    new pos = strfind(name, "_");
    if(pos <= 0 || name[pos + 1] == EOS)
    {
        format(dest, size, "%s", name);
        return 1;
    }
    new surname[MAX_PLAYER_NAME + 1];
    strmid(surname, name, pos + 1, strlen(name), sizeof surname);
    format(dest, size, "%c.%s", name[0], surname);
    return 1;
}

// Сколько символов остаётся на причину: итог "причина | By N.Name" должен влезть в SR_AF_REASON_LIMIT.
stock SRAdmin_AFReasonLimit(playerid)
{
    new author[40];
    SRAdmin_AFAuthor(GetPlayerNameEx(playerid), author, sizeof author);
    return SR_AF_REASON_LIMIT - 6 - strlen(author);         // 6 = длина " | By "
}

// Заменяет цветовые коды, управляющие символы и '%' на безопасные.
stock SRAdmin_AFCleanText(text[])
{
    for(new i, len = strlen(text); i < len; i++)
    {
        if(text[i] == '{') text[i] = '(';
        else if(text[i] == '}') text[i] = ')';
        else if(text[i] == '%') text[i] = '.';
        else if(text[i] < ' ') text[i] = ' ';
    }
    return 1;
}

stock SRAdmin_AFTrim(text[])
{
    new len = strlen(text);
    while(len > 0 && text[len - 1] == ' ') text[--len] = EOS;
    new start;
    while(text[start] == ' ') start++;
    if(start > 0) strdel(text, 0, start);
    return 1;
}

stock SRAdmin_AFValidNick(const nick[])
{
    new len = strlen(nick);
    if(len < 3 || len > MAX_PLAYER_NAME) return 0;
    for(new i; i < len; i++)
    {
        switch(nick[i])
        {
            case '0'..'9', 'a'..'z', 'A'..'Z', '_', '[', ']', '.', '(', ')', '$', '=', '@': continue;
            default: return 0;
        }
    }
    return 1;
}

stock SRAdmin_AFTermText(type, term, dest[], size)
{
    if(g_SRAFType[type][SRAT_HasTerm]) format(dest, size, "%d %s", term, g_SRAFType[type][SRAT_Unit]);
    else if(type == SR_AF_PERMBAN) format(dest, size, "Навсегда");
    else format(dest, size, "-");
    return 1;
}

// SendClientMessage режет строки около ~144 символов, поэтому длинное объявление делится на две строки.
stock SRAdmin_AFSendAll(color, const text[])
{
    new length = strlen(text);
    if(length <= 120) return SendClientMessageToAll(color, text);
    new cut = 110, first[128], second[192];
    while(cut > 60 && text[cut] != ' ') cut--;
    if(text[cut] != ' ') cut = 110;
    strmid(first, text, 0, cut, sizeof first);
    strmid(second, text, (text[cut] == ' ') ? cut + 1 : cut, length, sizeof second);
    SendClientMessageToAll(color, first);
    return SendClientMessageToAll(color, second);
}

stock SRAdmin_AFNotifyCreator(account, color, const text[])
{
    new creator = SRAdmin_AFFindOnline(account);
    if(creator != INVALID_PLAYER_ID) SendClientMessage(creator, color, text);
    return 1;
}

// Ищет аккаунт по нику. account - id аккаунта, level - админ-уровень, online - id игрока в сети
// (или INVALID_PLAYER_ID), name - ник в том виде, как он записан у игрока в сети (иначе как введён).
stock SRAdmin_AFResolve(const nick[], &account, &level, ip[], ip_size, &online, name[], name_size)
{
    new lookup[MAX_PLAYER_NAME + 1];
    format(lookup, sizeof lookup, "%s", nick);
    if(!SRAdmin_FindAccount(lookup, account, level, ip, ip_size)) return 0;
    if(level == 14) level = 13;
    online = SRAdmin_AFFindOnline(account);
    if(online != INVALID_PLAYER_ID) format(name, name_size, "%s", GetPlayerNameEx(online));
    else format(name, name_size, "%s", nick);
    return 1;
}

// ---------------------------------------------------------------------------------------------
// Создание формы
// ---------------------------------------------------------------------------------------------

// Общая проверка и создание формы. Возвращает 1 при успехе; при ошибке пишет причину игроку и возвращает 0.
stock SRAdmin_AFCreate(playerid, type, const nick[], term, const reason_in[])
{
    if(!SRAdmin_Require(playerid, SR_AF_MIN_LEVEL)) return 0;
    if(type < 0 || type >= SR_AF_TYPES) return 0;

    new text[192];
    if(!SRAdmin_AFValidNick(nick))
        return SRAdmin_AFError(playerid, "Укажите корректный ник игрока (например, Vasya_Pupkin).");

    if(g_SRAFType[type][SRAT_HasTerm])
    {
        if(term < g_SRAFType[type][SRAT_TermMin] || term > g_SRAFType[type][SRAT_TermMax])
        {
            format(text, sizeof text, "Срок для %s: от %d до %d %s",
                g_SRAFType[type][SRAT_Name], g_SRAFType[type][SRAT_TermMin], g_SRAFType[type][SRAT_TermMax], g_SRAFType[type][SRAT_Unit]);
            return SRAdmin_AFError(playerid, text);
        }
    }
    else term = 0;

    new reason[192];
    format(reason, sizeof reason, "%s", reason_in);
    SRAdmin_AFCleanText(reason);
    SRAdmin_AFTrim(reason);
    new limit = SRAdmin_AFReasonLimit(playerid);
    if(strlen(reason) < 3) return SRAdmin_AFError(playerid, "Причина слишком короткая (минимум 3 символа).");
    if(strlen(reason) > limit)
    {
        format(text, sizeof text, "Причина слишком длинная: максимум %d символов (в конец добавится \"| By ...\").", limit);
        return SRAdmin_AFError(playerid, text);
    }

    new account, level, ip[16], online, name[MAX_PLAYER_NAME + 1];
    if(!SRAdmin_AFResolve(nick, account, level, ip, sizeof ip, online, name, sizeof name))
        return SRAdmin_AFError(playerid, "Аккаунт с таким ником не найден.");
    new my_account = GetPlayerAccountID(playerid);
    if(account == my_account)
        return SRAdmin_AFError(playerid, "Нельзя подать форму на самого себя.");
    if(level > 0 && level >= SRAdmin_Level(playerid))
        return SRAdmin_AFError(playerid, "Нельзя подать форму на администратора своего или более высокого уровня.");
    if(g_SRAFType[type][SRAT_NeedOnline] && online == INVALID_PLAYER_ID)
    {
        format(text, sizeof text, "Игрок не в сети. Форма на %s подаётся только на игрока, который сейчас на сервере.", g_SRAFType[type][SRAT_Name]);
        return SRAdmin_AFError(playerid, text);
    }
    if(type == SR_AF_MUTE && GetPlayerData(online, P_MUTE) > 0)
        return SRAdmin_AFError(playerid, "У игрока уже есть мут.");
    if(type == SR_AF_JAIL && GetPlayerData(online, P_JAIL) > 0)
        return SRAdmin_AFError(playerid, "Игрок уже находится в тюрьме.");

    SRAdmin_AFPurge();
    new mine, free_slot = -1;
    for(new i; i < SR_AF_MAX_FORMS; i++)
    {
        if(!g_SRAFForm[i][SRAF_Serial])
        {
            if(free_slot == -1) free_slot = i;
            continue;
        }
        if(g_SRAFForm[i][SRAF_TargetAcc] == account && g_SRAFForm[i][SRAF_Type] == type)
            return SRAdmin_AFError(playerid, "На этого игрока уже подана такая форма.");
        if(g_SRAFForm[i][SRAF_CreatorAcc] == my_account) mine++;
    }
    if(mine >= SR_AF_MAX_PER_ADMIN)
        return SRAdmin_AFError(playerid, "У вас уже максимум активных форм. Дождитесь решения по предыдущим.");
    if(free_slot == -1)
        return SRAdmin_AFError(playerid, "Очередь форм переполнена. Попробуйте позже.");
    if(AntiSliv(playerid, "/af")) return 0;

    g_SRAFSerial++;
    g_SRAFForm[free_slot][SRAF_Serial] = g_SRAFSerial;
    g_SRAFForm[free_slot][SRAF_Time] = gettime();
    g_SRAFForm[free_slot][SRAF_Type] = type;
    g_SRAFForm[free_slot][SRAF_CreatorAcc] = my_account;
    format(g_SRAFForm[free_slot][SRAF_CreatorName], MAX_PLAYER_NAME + 1, "%s", GetPlayerNameEx(playerid));
    g_SRAFForm[free_slot][SRAF_TargetAcc] = account;
    format(g_SRAFForm[free_slot][SRAF_TargetName], MAX_PLAYER_NAME + 1, "%s", name);
    g_SRAFForm[free_slot][SRAF_Term] = term;
    format(g_SRAFForm[free_slot][SRAF_Reason], SR_AF_REASON_LIMIT + 1, "%s", reason);

    new message[256], term_text[24];
    SRAdmin_AFTermText(type, term, term_text, sizeof term_text);
    format(message, sizeof message, "[A] %s[%d] подал форму: %s на %s (/af).",
        GetPlayerNameEx(playerid), playerid, g_SRAFType[type][SRAT_Name], name);
    SendMessageToAdmins(message, SR_AF_COLOR_NEW, g_SRAFType[type][SRAT_Level]);

    format(message, sizeof message, "Форма подана: %s, %s, срок: %s. Принять её могут администраторы %d+ уровня.",
        g_SRAFType[type][SRAT_Name], name, term_text, g_SRAFType[type][SRAT_Level]);
    SendClientMessage(playerid, 0x66CC33FF, message);

    format(message, sizeof message, "AF submit: type=%s target=%s account=%d term=%d reason=%s",
        g_SRAFType[type][SRAT_Name], name, account, term, reason);
    SRAdmin_WriteLog(playerid, LOG_TYPE_ADMIN_ACTION, message);
    printf("[ADMIN] AF_SUBMIT actor=%d account=%d type=%d target_account=%d term=%d", playerid, my_account, type, account, term);
    return 1;
}

// Совместимость: /ban для администраторов 1-2 уровня создаёт форму Ban (вызывается из CMD:ban).
stock SRAdmin_AFSubmit(playerid, const params[])
{
    new targetid, days, reason[65];
    if(sscanf(params, "dds[65]", targetid, days, reason))
        return SendClientMessage(playerid, -1, "Используйте: /ban [id] [дней] [причина]. Бан администраторами 1-2 уровня выдаётся через форму (/af).");
    if(targetid < 0 || targetid >= MAX_PLAYERS || !IsPlayerConnected(targetid) || !IsPlayerLogged(targetid))
        return SendClientMessage(playerid, -1, "Игрок не найден.");
    new nick[MAX_PLAYER_NAME + 1];
    format(nick, sizeof nick, "%s", GetPlayerNameEx(targetid));
    SRAdmin_AFCreate(playerid, SR_AF_BAN, nick, days, reason);
    return 1;
}

// ---------------------------------------------------------------------------------------------
// Диалоги
// ---------------------------------------------------------------------------------------------

stock SRAdmin_AFShowMenu(playerid)
{
    SRAdmin_AFDraftClear(playerid);
    DeletePVar(playerid, "SRAfSerial");
    if(!SRAdmin_Require(playerid, SR_AF_MIN_LEVEL)) return 1;
    SRAdmin_AFPurge();
    new list[96];
    format(list, sizeof list, "Подать форму\nАктивные формы (%d)", SRAdmin_AFCount());
    SetPVarInt(playerid, "SRAfStage", SR_AF_STAGE_MENU);
    return Dialog(playerid, DIALOG_AF_LIST, DIALOG_STYLE_LIST, "Формы на наказания (/af)", list, "Выбрать", "Закрыть");
}

stock SRAdmin_AFShowTypes(playerid)
{
    new list[512], row[64];
    format(list, sizeof list, "Наказание\tПринимает администратор\n");
    for(new i; i < SR_AF_TYPES; i++)
    {
        format(row, sizeof row, "%s\t%d+ уровня\n", g_SRAFType[i][SRAT_Name], g_SRAFType[i][SRAT_Level]);
        strcat(list, row, sizeof list);
    }
    SetPVarInt(playerid, "SRAfStage", SR_AF_STAGE_TYPES);
    return Dialog(playerid, DIALOG_AF_LIST, DIALOG_STYLE_TABLIST_HEADERS, "Подать форму: выберите наказание", list, "Выбрать", "Назад");
}

// Какие строки есть в окне формы для данного типа (у Warn/Kick/Permban нет срока).
stock SRAdmin_AFFormRows(type, rows[])
{
    new count;
    rows[count++] = SR_AF_ROW_NICK;
    if(g_SRAFType[type][SRAT_HasTerm]) rows[count++] = SR_AF_ROW_TERM;
    rows[count++] = SR_AF_ROW_REASON;
    rows[count++] = SR_AF_ROW_SEND;
    rows[count++] = SR_AF_ROW_BACK;
    return count;
}

stock SRAdmin_AFShowForm(playerid)
{
    new type = GetPVarInt(playerid, "SRAfType");
    if(type < 0 || type >= SR_AF_TYPES) return SRAdmin_AFShowTypes(playerid);

    new nick[MAX_PLAYER_NAME + 1], reason[SR_AF_REASON_LIMIT + 1], term = GetPVarInt(playerid, "SRAfTerm");
    GetPVarString(playerid, "SRAfNick", nick, sizeof nick);
    GetPVarString(playerid, "SRAfReason", reason, sizeof reason);

    new list[1024], row[160], rows[8], count = SRAdmin_AFFormRows(type, rows);
    format(list, sizeof list, "Поле\tЗначение\n");
    for(new i; i < count; i++)
    {
        switch(rows[i])
        {
            case SR_AF_ROW_NICK:
            {
                if(strlen(nick)) format(row, sizeof row, "Ник\t{FFFFFF}%s\n", nick);
                else format(row, sizeof row, "Ник\t{AAAAAA}не указан\n");
            }
            case SR_AF_ROW_TERM:
            {
                if(term > 0) format(row, sizeof row, "Срок (%d-%d %s)\t{FFFFFF}%d %s\n",
                    g_SRAFType[type][SRAT_TermMin], g_SRAFType[type][SRAT_TermMax], g_SRAFType[type][SRAT_Unit], term, g_SRAFType[type][SRAT_Unit]);
                else format(row, sizeof row, "Срок (%d-%d %s)\t{AAAAAA}не указан\n",
                    g_SRAFType[type][SRAT_TermMin], g_SRAFType[type][SRAT_TermMax], g_SRAFType[type][SRAT_Unit]);
            }
            case SR_AF_ROW_REASON:
            {
                if(strlen(reason)) format(row, sizeof row, "Причина\t{FFFFFF}%s\n", reason);
                else format(row, sizeof row, "Причина\t{AAAAAA}не указана\n");
            }
            case SR_AF_ROW_SEND: format(row, sizeof row, "{66CC33}Отправить форму\t\n");
            default: format(row, sizeof row, "{AAAAAA}Назад\t\n");
        }
        strcat(list, row, sizeof list);
    }
    new title[80];
    format(title, sizeof title, "Форма: %s (принимает %d+ ур.)", g_SRAFType[type][SRAT_Name], g_SRAFType[type][SRAT_Level]);
    SetPVarInt(playerid, "SRAfStage", SR_AF_STAGE_FORM);
    return Dialog(playerid, DIALOG_AF_VIEW, DIALOG_STYLE_TABLIST_HEADERS, title, list, "Выбрать", "Назад");
}

stock SRAdmin_AFShowInput(playerid, field)
{
    new type = GetPVarInt(playerid, "SRAfType");
    if(type < 0 || type >= SR_AF_TYPES) return SRAdmin_AFShowTypes(playerid);

    new title[64], text[384];
    switch(field)
    {
        case SR_AF_ROW_NICK:
        {
            format(title, sizeof title, "%s: ник игрока", g_SRAFType[type][SRAT_Name]);
            format(text, sizeof text, "{FFFFFF}Введите ник игрока полностью, например: {FFCD00}Vasya_Pupkin");
        }
        case SR_AF_ROW_TERM:
        {
            format(title, sizeof title, "%s: срок", g_SRAFType[type][SRAT_Name]);
            format(text, sizeof text, "{FFFFFF}Введите срок числом: от {FFCD00}%d{FFFFFF} до {FFCD00}%d{FFFFFF} %s",
                g_SRAFType[type][SRAT_TermMin], g_SRAFType[type][SRAT_TermMax], g_SRAFType[type][SRAT_Unit]);
        }
        case SR_AF_ROW_REASON:
        {
            new author[40];
            SRAdmin_AFAuthor(GetPlayerNameEx(playerid), author, sizeof author);
            format(title, sizeof title, "%s: причина", g_SRAFType[type][SRAT_Name]);
            format(text, sizeof text, "{FFFFFF}Введите причину (до {FFCD00}%d{FFFFFF} символов).\nПри выдаче наказания добавится: {FFCD00}| By %s",
                SRAdmin_AFReasonLimit(playerid), author);
        }
        default: return SRAdmin_AFShowForm(playerid);
    }
    SetPVarInt(playerid, "SRAfField", field);
    SetPVarInt(playerid, "SRAfStage", SR_AF_STAGE_INPUT);
    return Dialog(playerid, DIALOG_AF_VIEW, DIALOG_STYLE_INPUT, title, text, "Готово", "Назад");
}

stock SRAdmin_AFShowList(playerid)
{
    if(!SRAdmin_Require(playerid, SR_AF_MIN_LEVEL)) return 1;
    SRAdmin_AFPurge();
    new list[4096], row[384], count, now = gettime();
    new level = SRAdmin_Level(playerid), my_account = GetPlayerAccountID(playerid);
    for(new i; i < SR_AF_MAX_FORMS; i++) g_SRAFRow[playerid][i] = 0;
    format(list, sizeof list, "Наказание\tИгрок\tСрок\tПричина\tАвтор\tВремя\n");
    for(new i; i < SR_AF_MAX_FORMS; i++)
    {
        if(!g_SRAFForm[i][SRAF_Serial]) continue;
        g_SRAFRow[playerid][count] = g_SRAFForm[i][SRAF_Serial];
        count++;
        new type = g_SRAFForm[i][SRAF_Type], color[10], term_text[24];
        // Серым выделены формы, которые этот администратор принять не может.
        if(level >= g_SRAFType[type][SRAT_Level] && my_account != g_SRAFForm[i][SRAF_CreatorAcc]) format(color, sizeof color, "{FFFFFF}");
        else format(color, sizeof color, "{777777}");
        SRAdmin_AFTermText(type, g_SRAFForm[i][SRAF_Term], term_text, sizeof term_text);
        format(row, sizeof row, "%s%s\t%s%s\t%s%s\t%s%s\t%s%s\t%s%d мин.\n",
            color, g_SRAFType[type][SRAT_Name],
            color, g_SRAFForm[i][SRAF_TargetName],
            color, term_text,
            color, g_SRAFForm[i][SRAF_Reason],
            color, g_SRAFForm[i][SRAF_CreatorName],
            color, (now - g_SRAFForm[i][SRAF_Time]) / 60);
        strcat(list, row, sizeof list);
    }
    if(!count)
    {
        SendClientMessage(playerid, 0xCECECEFF, "Активных форм сейчас нет.");
        return SRAdmin_AFShowMenu(playerid);
    }
    SetPVarInt(playerid, "SRAfStage", SR_AF_STAGE_LIST);
    return Dialog(playerid, DIALOG_AF_LIST, DIALOG_STYLE_TABLIST_HEADERS, "Активные формы", list, "Открыть", "Назад");
}

stock SRAdmin_AFShowView(playerid, serial)
{
    new idx = SRAdmin_AFFind(serial);
    if(idx == -1)
    {
        SendClientMessage(playerid, 0xCECECEFF, "Эта форма уже обработана или устарела.");
        return SRAdmin_AFShowList(playerid);
    }
    new type = g_SRAFForm[idx][SRAF_Type], required = g_SRAFType[type][SRAT_Level];
    new is_creator = (GetPlayerAccountID(playerid) == g_SRAFForm[idx][SRAF_CreatorAcc]);
    new can_accept = (!is_creator && SRAdmin_Level(playerid) >= required);
    new status[16], term_text[24], text[1024];
    if(SRAdmin_AFFindOnline(g_SRAFForm[idx][SRAF_TargetAcc]) != INVALID_PLAYER_ID) format(status, sizeof status, "В сети");
    else format(status, sizeof status, "Не в сети");
    SRAdmin_AFTermText(type, g_SRAFForm[idx][SRAF_Term], term_text, sizeof term_text);

    format(text, sizeof text, "Параметр\tЗначение\nНаказание\t%s (принимает %d+ ур.)\nИгрок\t%s\nСрок\t%s\nПричина\t%s\nАвтор формы\t%s\nСтатус игрока\t%s\n",
        g_SRAFType[type][SRAT_Name], required, g_SRAFForm[idx][SRAF_TargetName], term_text,
        g_SRAFForm[idx][SRAF_Reason], g_SRAFForm[idx][SRAF_CreatorName], status);
    if(can_accept) strcat(text, "{66CC33}Принять форму и выдать наказание\t\n", sizeof text);
    else strcat(text, "{777777}Принять форму (недоступно)\t\n", sizeof text);
    if(is_creator) strcat(text, "{FF5533}Отозвать свою форму\t\n", sizeof text);
    else if(SRAdmin_Level(playerid) >= required) strcat(text, "{FF5533}Отклонить форму\t\n", sizeof text);
    else strcat(text, "{777777}Отклонить форму (недоступно)\t\n", sizeof text);
    strcat(text, "{AAAAAA}Назад к списку\t\n", sizeof text);

    SetPVarInt(playerid, "SRAfStage", SR_AF_STAGE_VIEW);
    SetPVarInt(playerid, "SRAfSerial", serial);
    return Dialog(playerid, DIALOG_AF_VIEW, DIALOG_STYLE_TABLIST_HEADERS, "Форма на наказание", text, "Выбрать", "Назад");
}

// ---------------------------------------------------------------------------------------------
// Принятие / отклонение
// ---------------------------------------------------------------------------------------------

// Выдаёт наказание. Результат: 1 - выдано, 0 - не вышло (форма остаётся), 2 - наказание уже неактуально (форму нужно удалить).
stock SRAdmin_AFExecute(playerid, type, targetid, account, term, target_name[], ip[], reason[])
{
    new message[256];
    switch(type)
    {
        case SR_AF_MUTE:
        {
            if(GetPlayerData(targetid, P_MUTE) > 0)
            {
                SendClientMessage(playerid, -1, "У игрока уже есть мут. Форма удалена.");
                return 2;
            }
            if(AntiSliv(playerid, "/af")) return 0;
            if(!SRAdmin_SaveTimer(playerid, targetid, term * 60, 0))
            {
                SendClientMessage(playerid, -1, "Ошибка сохранения мута.");
                return 0;
            }
            SetPlayerData(targetid, P_MUTE, term * 60);
            format(message, sizeof message, "Администратор %s заблокировал чат %s на %d мин. Причина: %s",
                GetPlayerNameEx(playerid), GetPlayerNameEx(targetid), term, reason);
            SRAdmin_AFSendAll(SR_AF_COLOR_PUNISH, message);
            SRAdmin_WriteLog(playerid, LOG_TYPE_ADMIN_ACTION, message);
            return 1;
        }
        case SR_AF_JAIL:
        {
            if(GetPlayerData(targetid, P_JAIL) > 0)
            {
                SendClientMessage(playerid, -1, "Игрок уже находится в тюрьме. Форма удалена.");
                return 2;
            }
            if(AntiSliv(playerid, "/af")) return 0;
            if(!SRAdmin_SaveTimer(playerid, targetid, term * 60, 1))
            {
                SendClientMessage(playerid, -1, "Ошибка сохранения заключения.");
                return 0;
            }
            if(GetPlayerSpectateData(targetid, S_PLAYER) != -1) StopSpectate(targetid);
            JailPlayer(targetid, term);
            format(message, sizeof message, "Администратор %s отправил %s в тюрьму на %d мин. Причина: %s",
                GetPlayerNameEx(playerid), GetPlayerNameEx(targetid), term, reason);
            SRAdmin_AFSendAll(SR_AF_COLOR_PUNISH, message);
            SRAdmin_WriteLog(playerid, LOG_TYPE_ADMIN_ACTION, message);
            return 1;
        }
        case SR_AF_WARN:
        {
            new count;
            if(AntiSliv(playerid, "/af")) return 0;
            if(!SRAdmin_SaveWarning(playerid, targetid, 0, count))
            {
                SendClientMessage(playerid, -1, "Предупреждение не подтверждено БД. Проверьте серверный журнал.");
                return 0;
            }
            format(message, sizeof message, "Администратор %s выдал предупреждение %s [%d/3]. Причина: %s",
                GetPlayerNameEx(playerid), GetPlayerNameEx(targetid), count, reason);
            SRAdmin_AFSendAll(SR_AF_COLOR_PUNISH, message);
            SRAdmin_WriteLog(playerid, LOG_TYPE_ADMIN_ACTION, message);
            if(count >= 3) SendClientMessage(targetid, 0xFF5533FF, "Аккаунт заблокирован на 10 дней: получено 3 предупреждения.");
            SRAdmin_SafeKick(targetid);
            return 1;
        }
        case SR_AF_KICK:
        {
            if(AntiSliv(playerid, "/af")) return 0;
            format(message, sizeof message, "Администратор %s кикнул игрока %s. Причина: %s",
                GetPlayerNameEx(playerid), GetPlayerNameEx(targetid), reason);
            SRAdmin_AFSendAll(SR_AF_COLOR_PUNISH, message);
            SRAdmin_WriteLog(playerid, LOG_TYPE_ADMIN_ACTION, message);
            SRAdmin_SafeKick(targetid);
            return 1;
        }
        case SR_AF_BAN:
        {
            if(AntiSliv(playerid, "/af")) return 0;
            new result = SRAdmin_BanAccount(playerid, account, term, ip, reason);
            if(result == -1)
            {
                SendClientMessage(playerid, -1, "Аккаунт уже заблокирован. Форма удалена.");
                return 2;
            }
            if(result != 1)
            {
                SendClientMessage(playerid, -1, "Блокировка не подтверждена БД. Проверьте серверный журнал.");
                return 0;
            }
            format(message, sizeof message, "Администратор %s заблокировал %s на %d дней. Причина: %s",
                GetPlayerNameEx(playerid), target_name, term, reason);
            SRAdmin_AFSendAll(SR_AF_COLOR_PUNISH, message);
            SRAdmin_WriteLog(playerid, LOG_TYPE_SUPERADMIN_ACTION, message);
            if(targetid != INVALID_PLAYER_ID && IsPlayerConnected(targetid) && GetPlayerAccountID(targetid) == account)
                SRAdmin_SafeKick(targetid);
            return 1;
        }
        case SR_AF_PERMBAN:
        {
            if(AntiSliv(playerid, "/af")) return 0;
            if(!AddBan(account, gettime(), SR_AF_PERMBAN_DAYS, ip, reason, GetPlayerNameEx(playerid)))
            {
                SendClientMessage(playerid, -1, "Блокировка не подтверждена БД. Проверьте серверный журнал.");
                return 0;
            }
            InfoAstats[1] += 1;
            format(message, sizeof message, "Администратор %s навсегда заблокировал игрока %s. Причина: %s",
                GetPlayerNameEx(playerid), target_name, reason);
            SRAdmin_AFSendAll(SR_AF_COLOR_PUNISH, message);
            SRAdmin_WriteLog(playerid, LOG_TYPE_SUPERADMIN_ACTION, message);
            if(targetid != INVALID_PLAYER_ID && IsPlayerConnected(targetid) && GetPlayerAccountID(targetid) == account)
                SRAdmin_SafeKick(targetid);
            return 1;
        }
    }
    return 0;
}

stock SRAdmin_AFAccept(playerid, serial)
{
    new idx = SRAdmin_AFFind(serial);
    if(idx == -1)
    {
        SendClientMessage(playerid, 0xCECECEFF, "Форма уже обработана или устарела.");
        return SRAdmin_AFShowList(playerid);
    }
    new type = g_SRAFForm[idx][SRAF_Type], required = g_SRAFType[type][SRAT_Level], message[192];
    if(SRAdmin_Level(playerid) < required)
    {
        format(message, sizeof message, "Для принятия формы %s нужен %d уровень администратора.", g_SRAFType[type][SRAT_Name], required);
        return SendClientMessage(playerid, 0xFF5533FF, message);
    }
    if(!SRAdmin_Require(playerid, required)) return 1;
    if(GetPlayerAccountID(playerid) == g_SRAFForm[idx][SRAF_CreatorAcc])
        return SendClientMessage(playerid, 0xFF5533FF, "Нельзя принять собственную форму.");

    new account = g_SRAFForm[idx][SRAF_TargetAcc], term = g_SRAFForm[idx][SRAF_Term], creator_account = g_SRAFForm[idx][SRAF_CreatorAcc];
    new target_name[MAX_PLAYER_NAME + 1], creator_name[MAX_PLAYER_NAME + 1], base_reason[SR_AF_REASON_LIMIT + 1];
    new reason[SR_AF_REASON_LIMIT + 1], author[40], ip[16];
    format(target_name, sizeof target_name, "%s", g_SRAFForm[idx][SRAF_TargetName]);
    format(creator_name, sizeof creator_name, "%s", g_SRAFForm[idx][SRAF_CreatorName]);
    format(base_reason, sizeof base_reason, "%s", g_SRAFForm[idx][SRAF_Reason]);
    // Причина наказания: "Причина | By N.Name"
    SRAdmin_AFAuthor(creator_name, author, sizeof author);
    format(reason, sizeof reason, "%s | By %s", base_reason, author);

    new targetid = SRAdmin_AFFindOnline(account);
    if(targetid != INVALID_PLAYER_ID)
    {
        if(!SRAdmin_Target(playerid, targetid, required)) return 1;
        format(ip, sizeof ip, "%s", GetPlayerIpEx(targetid));
    }
    else
    {
        if(g_SRAFType[type][SRAT_NeedOnline])
            return SendClientMessage(playerid, -1, "Игрок сейчас не в сети. Принять эту форму можно, когда он зайдёт на сервер.");
        new found_account, level;
        if(!SRAdmin_FindAccount(target_name, found_account, level, ip, sizeof ip) || found_account != account)
            return SendClientMessage(playerid, -1, "Аккаунт игрока не найден в базе данных.");
        if(level == 14) level = 13;
        if(level < 0 || level > SRAdmin_Level(playerid))
            return SendClientMessage(playerid, -1, "Недостаточно прав для наказания этого аккаунта.");
    }

    new result = SRAdmin_AFExecute(playerid, type, targetid, account, term, target_name, ip, reason);
    if(!result) return 1;
    g_SRAFForm[idx][SRAF_Serial] = 0;
    if(result != 1) return 1;

    format(message, sizeof message, "AF %s accepted: target=%s account=%d term=%d submitted_by=%s account=%d reason=%s",
        g_SRAFType[type][SRAT_Name], target_name, account, term, creator_name, creator_account, reason);
    SRAdmin_WriteLog(playerid, LOG_TYPE_SUPERADMIN_ACTION, message);
    format(message, sizeof message, "Ваша форма %s на игрока %s принята администратором %s.",
        g_SRAFType[type][SRAT_Name], target_name, GetPlayerNameEx(playerid));
    SRAdmin_AFNotifyCreator(creator_account, 0x66CC33FF, message);
    return 1;
}

stock SRAdmin_AFDecline(playerid, serial)
{
    new idx = SRAdmin_AFFind(serial);
    if(idx == -1)
    {
        SendClientMessage(playerid, 0xCECECEFF, "Форма уже обработана или устарела.");
        return SRAdmin_AFShowList(playerid);
    }
    if(!SRAdmin_Require(playerid, SR_AF_MIN_LEVEL)) return 1;

    new type = g_SRAFForm[idx][SRAF_Type], required = g_SRAFType[type][SRAT_Level], creator_account = g_SRAFForm[idx][SRAF_CreatorAcc];
    new is_creator = (GetPlayerAccountID(playerid) == creator_account), message[192];
    if(!is_creator && SRAdmin_Level(playerid) < required)
    {
        format(message, sizeof message, "Отклонить форму %s может её автор или администратор %d+ уровня.", g_SRAFType[type][SRAT_Name], required);
        return SendClientMessage(playerid, 0xFF5533FF, message);
    }

    new target_name[MAX_PLAYER_NAME + 1], creator_name[MAX_PLAYER_NAME + 1];
    format(target_name, sizeof target_name, "%s", g_SRAFForm[idx][SRAF_TargetName]);
    format(creator_name, sizeof creator_name, "%s", g_SRAFForm[idx][SRAF_CreatorName]);
    g_SRAFForm[idx][SRAF_Serial] = 0;

    if(is_creator) format(message, sizeof message, "[A] %s[%d] отозвал свою форму %s на игрока %s.",
        GetPlayerNameEx(playerid), playerid, g_SRAFType[type][SRAT_Name], target_name);
    else format(message, sizeof message, "[A] %s[%d] отклонил форму %s на игрока %s (автор %s).",
        GetPlayerNameEx(playerid), playerid, g_SRAFType[type][SRAT_Name], target_name, creator_name);
    SendMessageToAdmins(message, SR_AF_COLOR_NEW, required);
    SRAdmin_WriteLog(playerid, LOG_TYPE_ADMIN_ACTION, message);

    if(!is_creator)
    {
        format(message, sizeof message, "Ваша форма %s на игрока %s отклонена администратором %s.",
            g_SRAFType[type][SRAT_Name], target_name, GetPlayerNameEx(playerid));
        SRAdmin_AFNotifyCreator(creator_account, 0xFF5533FF, message);
    }
    return 1;
}

// ---------------------------------------------------------------------------------------------
// Обработка ответов диалогов (вызывается из OnDialogResponse для DIALOG_AF_LIST и DIALOG_AF_VIEW)
// ---------------------------------------------------------------------------------------------

stock SRAdmin_AFDialogResponse(playerid, dialogid, response, listitem, const inputtext[] = "")
{
    new stage = GetPVarInt(playerid, "SRAfStage");
    DeletePVar(playerid, "SRAfStage");
    if(dialogid == DIALOG_AF_LIST)
    {
        if(stage < SR_AF_STAGE_MENU || stage > SR_AF_STAGE_LIST) return 1;
    }
    else if(dialogid == DIALOG_AF_VIEW)
    {
        if(stage < SR_AF_STAGE_FORM || stage > SR_AF_STAGE_VIEW) return 1;
    }
    else return 1;

    if(!SRAdmin_Require(playerid, SR_AF_MIN_LEVEL))
    {
        SRAdmin_AFReset(playerid);
        return 1;
    }

    switch(stage)
    {
        case SR_AF_STAGE_MENU:
        {
            if(!response)
            {
                SRAdmin_AFReset(playerid);
                return 1;
            }
            if(listitem == 0) return SRAdmin_AFShowTypes(playerid);
            if(listitem == 1) return SRAdmin_AFShowList(playerid);
            return SRAdmin_AFShowMenu(playerid);
        }
        case SR_AF_STAGE_TYPES:
        {
            if(!response) return SRAdmin_AFShowMenu(playerid);
            if(listitem < 0 || listitem >= SR_AF_TYPES) return SRAdmin_AFShowTypes(playerid);
            SRAdmin_AFDraftClear(playerid);
            SetPVarInt(playerid, "SRAfType", listitem);
            return SRAdmin_AFShowForm(playerid);
        }
        case SR_AF_STAGE_LIST:
        {
            if(!response) return SRAdmin_AFShowMenu(playerid);
            if(listitem < 0 || listitem >= SR_AF_MAX_FORMS) return SRAdmin_AFShowList(playerid);
            new serial = g_SRAFRow[playerid][listitem];
            if(!serial) return SRAdmin_AFShowList(playerid);
            return SRAdmin_AFShowView(playerid, serial);
        }
        case SR_AF_STAGE_FORM:
        {
            if(!response) return SRAdmin_AFShowTypes(playerid);
            new type = GetPVarInt(playerid, "SRAfType");
            if(type < 0 || type >= SR_AF_TYPES) return SRAdmin_AFShowTypes(playerid);
            new rows[8], count = SRAdmin_AFFormRows(type, rows);
            if(listitem < 0 || listitem >= count) return SRAdmin_AFShowForm(playerid);
            switch(rows[listitem])
            {
                case SR_AF_ROW_NICK, SR_AF_ROW_TERM, SR_AF_ROW_REASON: return SRAdmin_AFShowInput(playerid, rows[listitem]);
                case SR_AF_ROW_SEND:
                {
                    new nick[MAX_PLAYER_NAME + 1], reason[SR_AF_REASON_LIMIT + 1], term = GetPVarInt(playerid, "SRAfTerm");
                    GetPVarString(playerid, "SRAfNick", nick, sizeof nick);
                    GetPVarString(playerid, "SRAfReason", reason, sizeof reason);
                    if(!strlen(nick))
                    {
                        SendClientMessage(playerid, -1, "Укажите ник игрока.");
                        return SRAdmin_AFShowForm(playerid);
                    }
                    if(g_SRAFType[type][SRAT_HasTerm] && term <= 0)
                    {
                        SendClientMessage(playerid, -1, "Укажите срок наказания.");
                        return SRAdmin_AFShowForm(playerid);
                    }
                    if(!strlen(reason))
                    {
                        SendClientMessage(playerid, -1, "Укажите причину.");
                        return SRAdmin_AFShowForm(playerid);
                    }
                    if(SRAdmin_AFCreate(playerid, type, nick, term, reason))
                    {
                        SRAdmin_AFReset(playerid);
                        return 1;
                    }
                    return SRAdmin_AFShowForm(playerid);
                }
                default: return SRAdmin_AFShowTypes(playerid);
            }
        }
        case SR_AF_STAGE_INPUT:
        {
            if(!response) return SRAdmin_AFShowForm(playerid);
            new field = GetPVarInt(playerid, "SRAfField"), type = GetPVarInt(playerid, "SRAfType");
            if(type < 0 || type >= SR_AF_TYPES) return SRAdmin_AFShowTypes(playerid);

            new text[192];
            format(text, sizeof text, "%s", inputtext);
            utf8_to_cp1251(text);
            SRAdmin_AFCleanText(text);
            SRAdmin_AFTrim(text);

            switch(field)
            {
                case SR_AF_ROW_NICK:
                {
                    if(!SRAdmin_AFValidNick(text))
                    {
                        SendClientMessage(playerid, -1, "Некорректный ник. Пример: Vasya_Pupkin.");
                        return SRAdmin_AFShowInput(playerid, field);
                    }
                    SetPVarString(playerid, "SRAfNick", text);
                }
                case SR_AF_ROW_TERM:
                {
                    new length = strlen(text), numeric = (length > 0 && length <= 4);
                    for(new i; i < length; i++)
                        if(text[i] < '0' || text[i] > '9') numeric = 0;
                    new value = numeric ? strval(text) : 0;
                    if(value < g_SRAFType[type][SRAT_TermMin] || value > g_SRAFType[type][SRAT_TermMax])
                    {
                        new error[96];
                        format(error, sizeof error, "Срок должен быть числом от %d до %d %s.",
                            g_SRAFType[type][SRAT_TermMin], g_SRAFType[type][SRAT_TermMax], g_SRAFType[type][SRAT_Unit]);
                        SendClientMessage(playerid, -1, error);
                        return SRAdmin_AFShowInput(playerid, field);
                    }
                    SetPVarInt(playerid, "SRAfTerm", value);
                }
                case SR_AF_ROW_REASON:
                {
                    new limit = SRAdmin_AFReasonLimit(playerid);
                    if(strlen(text) < 3)
                    {
                        SendClientMessage(playerid, -1, "Причина слишком короткая (минимум 3 символа).");
                        return SRAdmin_AFShowInput(playerid, field);
                    }
                    if(strlen(text) > limit)
                    {
                        new error[96];
                        format(error, sizeof error, "Причина слишком длинная: максимум %d символов.", limit);
                        SendClientMessage(playerid, -1, error);
                        return SRAdmin_AFShowInput(playerid, field);
                    }
                    SetPVarString(playerid, "SRAfReason", text);
                }
            }
            return SRAdmin_AFShowForm(playerid);
        }
        case SR_AF_STAGE_VIEW:
        {
            new serial = GetPVarInt(playerid, "SRAfSerial");
            DeletePVar(playerid, "SRAfSerial");
            if(!response) return SRAdmin_AFShowList(playerid);
            if(listitem == SR_AF_VIEW_INFO_ROWS) return SRAdmin_AFAccept(playerid, serial);
            if(listitem == SR_AF_VIEW_INFO_ROWS + 1) return SRAdmin_AFDecline(playerid, serial);
            if(listitem == SR_AF_VIEW_INFO_ROWS + 2) return SRAdmin_AFShowList(playerid);
            return SRAdmin_AFShowView(playerid, serial);
        }
    }
    return 1;
}

CMD:af(playerid, params[])
{
    #pragma unused params
    return SRAdmin_AFShowMenu(playerid);
}
