// Система форм на бан /af.
// Администраторы 1-2 уровня не банят сами: /ban и кнопка BAN в /sp создают форму.
// Администратор 3+ уровня открывает /af, принимает форму (бан выдаётся от его имени) или отказывает.
#if defined _sr_admin_af_included
    #endinput
#endif
#define _sr_admin_af_included

#if !defined DIALOG_AF_LIST
    #define DIALOG_AF_LIST      (31901)
    #define DIALOG_AF_VIEW      (31902)
#endif

#define SR_AF_MAX_FORMS         (20)        // всего форм в очереди
#define SR_AF_MAX_PER_ADMIN     (3)         // активных форм от одного админа
#define SR_AF_LIFETIME          (1800)      // форма живёт 30 минут
#define SR_AF_MAX_DAYS          (30)        // максимум дней в форме
#define SR_AF_COLOR_NEW         (0xFFAA00FF)
#define SR_AF_COLOR_BAN         (0xFF5533FF)

enum E_SR_AF_FORM
{
    SRAF_Serial,                            // 0 = слот свободен
    SRAF_Time,
    SRAF_CreatorAcc,
    SRAF_CreatorName[MAX_PLAYER_NAME + 1],
    SRAF_TargetAcc,
    SRAF_TargetName[MAX_PLAYER_NAME + 1],
    SRAF_Days,
    SRAF_Reason[65]
};

new g_SRAFForm[SR_AF_MAX_FORMS][E_SR_AF_FORM];
new g_SRAFSerial;
new g_SRAFRow[MAX_PLAYERS][SR_AF_MAX_FORMS];

stock SRAdmin_AFReset(playerid)
{
    DeletePVar(playerid, "SRAfStage");
    DeletePVar(playerid, "SRAfSerial");
    for(new i; i < SR_AF_MAX_FORMS; i++) g_SRAFRow[playerid][i] = 0;
    return 1;
}

stock SRAdmin_AFPurge()
{
    new now = gettime();
    for(new i; i < SR_AF_MAX_FORMS; i++)
        if(g_SRAFForm[i][SRAF_Serial] && now - g_SRAFForm[i][SRAF_Time] > SR_AF_LIFETIME)
            g_SRAFForm[i][SRAF_Serial] = 0;
    return 1;
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

// Vasya_Pupkin -> "V. Pupkin"
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
    format(dest, size, "%c. %s", name[0], surname);
    return 1;
}

// Убирает цветовые коды, управляющие символы и '%' из причины.
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

// SendClientMessage режет строки длиннее ~144 символов, поэтому длинный текст делим на две строки.
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

// Вызывается из /ban (и кнопки BAN в /sp) для администраторов 1-2 уровня.
stock SRAdmin_AFSubmit(playerid, const params[])
{
    new targetid, days, reason[65];
    if(sscanf(params, "dds[65]", targetid, days, reason))
        return SendClientMessage(playerid, -1, "Используйте: /ban [id] [дней] [причина]. Бан выдаст администратор 3+ уровня после принятия формы (/af).");

    if(targetid < 0 || targetid >= MAX_PLAYERS || targetid == playerid || !IsPlayerConnected(targetid) || !IsPlayerLogged(targetid))
        return SendClientMessage(playerid, -1, "Игрок не найден.");
    new target_admin = GetPlayerAdminEx(targetid);
    if(target_admin > 0 && target_admin >= SRAdmin_Level(playerid))
        return SendClientMessage(playerid, 0xFF5533FF, "Нельзя подать форму на администратора вашего или более высокого уровня.");
    if(days < 1 || days > SR_AF_MAX_DAYS)
    {
        new text[96];
        format(text, sizeof text, "Допустимый срок в форме: от 1 до %d дней.", SR_AF_MAX_DAYS);
        return SendClientMessage(playerid, -1, text);
    }
    SRAdmin_AFCleanText(reason);
    if(strlen(reason) < 3) return SendClientMessage(playerid, -1, "Укажите причину бана (минимум 3 символа).");

    SRAdmin_AFPurge();
    new account = GetPlayerAccountID(playerid), target_account = GetPlayerAccountID(targetid), mine, free_slot = -1;
    for(new i; i < SR_AF_MAX_FORMS; i++)
    {
        if(!g_SRAFForm[i][SRAF_Serial])
        {
            if(free_slot == -1) free_slot = i;
            continue;
        }
        if(g_SRAFForm[i][SRAF_TargetAcc] == target_account)
            return SendClientMessage(playerid, -1, "На этого игрока уже подана форма на бан.");
        if(g_SRAFForm[i][SRAF_CreatorAcc] == account) mine++;
    }
    if(mine >= SR_AF_MAX_PER_ADMIN)
        return SendClientMessage(playerid, -1, "У вас уже максимум активных форм. Дождитесь решения по предыдущим.");
    if(free_slot == -1)
        return SendClientMessage(playerid, -1, "Очередь форм переполнена. Попробуйте позже.");
    if(AntiSliv(playerid, "/ban")) return 1;

    g_SRAFSerial++;
    g_SRAFForm[free_slot][SRAF_Serial] = g_SRAFSerial;
    g_SRAFForm[free_slot][SRAF_Time] = gettime();
    g_SRAFForm[free_slot][SRAF_CreatorAcc] = account;
    format(g_SRAFForm[free_slot][SRAF_CreatorName], MAX_PLAYER_NAME + 1, "%s", GetPlayerNameEx(playerid));
    g_SRAFForm[free_slot][SRAF_TargetAcc] = target_account;
    format(g_SRAFForm[free_slot][SRAF_TargetName], MAX_PLAYER_NAME + 1, "%s", GetPlayerNameEx(targetid));
    g_SRAFForm[free_slot][SRAF_Days] = days;
    format(g_SRAFForm[free_slot][SRAF_Reason], 65, "%s", reason);

    new message[192];
    format(message, sizeof message, "[A] %s[%d] внес новую форму для администрации сервера (/af).", GetPlayerNameEx(playerid), playerid);
    SendMessageToAdmins(message, SR_AF_COLOR_NEW);

    format(message, sizeof message, "Форма отправлена: %s, %d дн., причина: %s. Ожидайте решения администратора 3+ уровня.", GetPlayerNameEx(targetid), days, reason);
    SendClientMessage(playerid, 0x66CC33FF, message);

    format(message, sizeof message, "AF submit: target=%s account=%d days=%d reason=%s", GetPlayerNameEx(targetid), target_account, days, reason);
    SRAdmin_WriteLog(playerid, LOG_TYPE_ADMIN_ACTION, message);
    printf("[ADMIN] AF_SUBMIT actor=%d account=%d target_account=%d days=%d", playerid, account, target_account, days);
    return 1;
}

stock SRAdmin_AFShowList(playerid)
{
    DeletePVar(playerid, "SRAfStage");
    if(!SRAdmin_Require(playerid, 3)) return 1;
    SRAdmin_AFPurge();
    new list[4096], row[256], count, now = gettime();
    format(list, sizeof list, "№\tИгрок\tСрок\tПричина\tПодал\tЖдёт\n");
    for(new i; i < SR_AF_MAX_FORMS; i++) g_SRAFRow[playerid][i] = 0;
    for(new i; i < SR_AF_MAX_FORMS; i++)
    {
        if(!g_SRAFForm[i][SRAF_Serial]) continue;
        g_SRAFRow[playerid][count] = g_SRAFForm[i][SRAF_Serial];
        count++;
        format(row, sizeof row, "%d\t%s\t%d дн.\t%s\t%s\t%d мин.\n", count,
            g_SRAFForm[i][SRAF_TargetName], g_SRAFForm[i][SRAF_Days], g_SRAFForm[i][SRAF_Reason],
            g_SRAFForm[i][SRAF_CreatorName], (now - g_SRAFForm[i][SRAF_Time]) / 60);
        strcat(list, row, sizeof list);
    }
    if(!count) return SendClientMessage(playerid, 0xCECECEFF, "Активных форм на бан нет.");
    SetPVarInt(playerid, "SRAfStage", 1);
    return Dialog(playerid, DIALOG_AF_LIST, DIALOG_STYLE_TABLIST_HEADERS, "Формы на бан (/af)", list, "Открыть", "Закрыть");
}

stock SRAdmin_AFShowView(playerid, serial)
{
    new idx = SRAdmin_AFFind(serial);
    if(idx == -1)
    {
        SendClientMessage(playerid, 0xCECECEFF, "Эта форма уже обработана или устарела.");
        return SRAdmin_AFShowList(playerid);
    }
    new status[16], text[768];
    if(SRAdmin_AFFindOnline(g_SRAFForm[idx][SRAF_TargetAcc]) != INVALID_PLAYER_ID) format(status, sizeof status, "в сети");
    else format(status, sizeof status, "не в сети");
    format(text, sizeof text, "Параметр\tЗначение\nИгрок\t%s\nСрок\t%d дн.\nПричина\t%s\nПодал\t%s\nСтатус игрока\t%s\n",
        g_SRAFForm[idx][SRAF_TargetName], g_SRAFForm[idx][SRAF_Days], g_SRAFForm[idx][SRAF_Reason],
        g_SRAFForm[idx][SRAF_CreatorName], status);
    strcat(text, "{66CC33}Принять форму и выдать бан\t\n{FF5533}Отказать в форме\t\n{AAAAAA}Назад к списку\t\n", sizeof text);
    SetPVarInt(playerid, "SRAfStage", 2);
    SetPVarInt(playerid, "SRAfSerial", serial);
    return Dialog(playerid, DIALOG_AF_VIEW, DIALOG_STYLE_TABLIST_HEADERS, "Форма на бан", text, "Выбрать", "Закрыть");
}

stock SRAdmin_AFNotifyCreator(account, color, const text[])
{
    new creator = SRAdmin_AFFindOnline(account);
    if(creator != INVALID_PLAYER_ID) SendClientMessage(creator, color, text);
    return 1;
}

stock SRAdmin_AFAccept(playerid, serial)
{
    new idx = SRAdmin_AFFind(serial);
    if(idx == -1) return SendClientMessage(playerid, 0xCECECEFF, "Форма уже обработана или устарела.");
    if(!SRAdmin_Require(playerid, 3)) return 1;
    if(GetPlayerAccountID(playerid) == g_SRAFForm[idx][SRAF_CreatorAcc])
        return SendClientMessage(playerid, 0xFF5533FF, "Нельзя принять собственную форму.");

    new account = g_SRAFForm[idx][SRAF_TargetAcc], days = g_SRAFForm[idx][SRAF_Days];
    new target_name[MAX_PLAYER_NAME + 1], creator_name[MAX_PLAYER_NAME + 1], reason[65], ip[16];
    format(target_name, sizeof target_name, "%s", g_SRAFForm[idx][SRAF_TargetName]);
    format(creator_name, sizeof creator_name, "%s", g_SRAFForm[idx][SRAF_CreatorName]);
    format(reason, sizeof reason, "%s", g_SRAFForm[idx][SRAF_Reason]);
    new creator_account = g_SRAFForm[idx][SRAF_CreatorAcc];

    new targetid = SRAdmin_AFFindOnline(account);
    if(targetid != INVALID_PLAYER_ID)
    {
        if(!SRAdmin_Target(playerid, targetid, 3)) return 1;
        format(ip, sizeof ip, "%s", GetPlayerIpEx(targetid));
    }
    else
    {
        new found_account, level;
        if(!SRAdmin_FindAccount(target_name, found_account, level, ip, sizeof ip) || found_account != account)
            return SendClientMessage(playerid, -1, "Аккаунт не найден либо БД недоступна.");
        if(level == 14) level = 13;
        if(level < 0 || level > SRAdmin_Level(playerid))
            return SendClientMessage(playerid, -1, "Недостаточно прав для блокировки этого аккаунта.");
    }
    if(AntiSliv(playerid, "/af")) return 1;

    new result = SRAdmin_BanAccount(playerid, account, days, ip, reason);
    if(result == -1)
    {
        g_SRAFForm[idx][SRAF_Serial] = 0;
        return SendClientMessage(playerid, -1, "Аккаунт уже заблокирован. Форма удалена.");
    }
    if(result != 1) return SendClientMessage(playerid, -1, "Блокировка не подтверждена БД. Проверьте серверный журнал.");
    g_SRAFForm[idx][SRAF_Serial] = 0;

    new author[40], message[256];
    SRAdmin_AFAuthor(creator_name, author, sizeof author);
    format(message, sizeof message, "Администратор %s заблокировал игрока %s на %d дн. Причина: %s | by %s",
        GetPlayerNameEx(playerid), target_name, days, reason, author);
    SRAdmin_AFSendAll(SR_AF_COLOR_BAN, message);

    format(message, sizeof message, "AF ban accepted: target=%s account=%d days=%d submitted_by=%s account=%d reason=%s",
        target_name, account, days, creator_name, creator_account, reason);
    SRAdmin_WriteLog(playerid, LOG_TYPE_SUPERADMIN_ACTION, message);

    format(message, sizeof message, "Ваша форма на бан %s принята администратором %s.", target_name, GetPlayerNameEx(playerid));
    SRAdmin_AFNotifyCreator(creator_account, 0x66CC33FF, message);

    if(targetid != INVALID_PLAYER_ID && IsPlayerConnected(targetid) && GetPlayerAccountID(targetid) == account)
        SRAdmin_SafeKick(targetid);
    return 1;
}

stock SRAdmin_AFDecline(playerid, serial)
{
    new idx = SRAdmin_AFFind(serial);
    if(idx == -1) return SendClientMessage(playerid, 0xCECECEFF, "Форма уже обработана или устарела.");
    if(!SRAdmin_Require(playerid, 3)) return 1;

    new target_name[MAX_PLAYER_NAME + 1], creator_name[MAX_PLAYER_NAME + 1], creator_account = g_SRAFForm[idx][SRAF_CreatorAcc];
    format(target_name, sizeof target_name, "%s", g_SRAFForm[idx][SRAF_TargetName]);
    format(creator_name, sizeof creator_name, "%s", g_SRAFForm[idx][SRAF_CreatorName]);
    g_SRAFForm[idx][SRAF_Serial] = 0;

    new message[192];
    format(message, sizeof message, "[A] %s[%d] отказал в форме на бан %s (подал %s).", GetPlayerNameEx(playerid), playerid, target_name, creator_name);
    SendMessageToAdmins(message, SR_AF_COLOR_NEW);
    SRAdmin_WriteLog(playerid, LOG_TYPE_ADMIN_ACTION, message);

    format(message, sizeof message, "Ваша форма на бан %s отклонена администратором %s.", target_name, GetPlayerNameEx(playerid));
    SRAdmin_AFNotifyCreator(creator_account, 0xFF5533FF, message);
    return 1;
}

stock SRAdmin_AFDialogResponse(playerid, dialogid, response, listitem)
{
    new stage = GetPVarInt(playerid, "SRAfStage");
    if(dialogid == DIALOG_AF_LIST)
    {
        if(stage != 1) return 1;
        DeletePVar(playerid, "SRAfStage");
        if(!response) return 1;
        if(!SRAdmin_Require(playerid, 3)) return 1;
        if(listitem < 0 || listitem >= SR_AF_MAX_FORMS) return 1;
        new serial = g_SRAFRow[playerid][listitem];
        if(!serial) return 1;
        return SRAdmin_AFShowView(playerid, serial);
    }
    if(dialogid == DIALOG_AF_VIEW)
    {
        if(stage != 2) return 1;
        new serial = GetPVarInt(playerid, "SRAfSerial");
        DeletePVar(playerid, "SRAfStage");
        DeletePVar(playerid, "SRAfSerial");
        if(!response) return 1;
        if(!SRAdmin_Require(playerid, 3)) return 1;
        if(listitem == 5) return SRAdmin_AFAccept(playerid, serial);
        if(listitem == 6) return SRAdmin_AFDecline(playerid, serial);
        if(listitem == 7) return SRAdmin_AFShowList(playerid);
        return SRAdmin_AFShowView(playerid, serial);
    }
    return 1;
}

CMD:af(playerid, params[])
{
    #pragma unused params
    return SRAdmin_AFShowList(playerid);
}
