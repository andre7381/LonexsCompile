// /setadmin: блоки уровней -> должности -> применение. Модуль подключается ПОСЛЕ admin_skarf_core.inc и admin_skarf_menu.inc.
// Хранение: accounts.admin = уровень (как раньше), accounts.admin_pos = номер должности (1..SR_ADMIN_POS_COUNT, 0 = не задана).
// Колонка admin_pos создаётся автоматически в CREATE_ACCOUNTS_TABLE (icerussia.pwn).
// Номер должности игрока хранится в массиве g_SRAdminPosOf[playerid] (объявлен в icerussia.pwn рядом с DIALOG_SETADMIN_*).
// ВАЖНО: порядок строк в g_SRAdminPos НЕ менять и не вставлять строки в середину - в БД хранится номер строки.
// Новые должности добавлять только в КОНЕЦ таблицы (и увеличивать SR_ADMIN_POS_COUNT).
#if defined _sr_admin_setadmin_included
    #endinput
#endif
#define _sr_admin_setadmin_included

#if !defined DIALOG_SETADMIN_POS
    #define DIALOG_SETADMIN_POS     (31905)
    #define DIALOG_SETADMIN_INFO    (31906)
#endif

// /setadmin доступен с этого уровня.
#define SR_SETADMIN_MIN_LEVEL       (7)

// Максимальный уровень, который может выдавать/менять администратор данного уровня:
// 7 ур. - только 6 уровень; 8 ур. - до 7 уровня; 9+ ур. - до своего уровня включительно (как было раньше).
stock SRSet_Max(actor_level)
{
    if(actor_level < SR_SETADMIN_MIN_LEVEL) return 0;
    if(actor_level == 7) return 6;
    if(actor_level == 8) return 7;
    return actor_level;
}

#define SR_ADMIN_POS_COUNT          (52)
#define SR_ADMIN_POS_NAME_LEN       (40)

enum E_SR_ADMIN_POS { SRP_LEVEL, SRP_NAME[SR_ADMIN_POS_NAME_LEN] };
new const g_SRAdminPos[SR_ADMIN_POS_COUNT][E_SR_ADMIN_POS] =
{
    {1, "Младший модератор"},
    {1, "Блогер"},
    {2, "Модератор"},
    {3, "Старший модератор"},
    {3, "Медиа"},
    {4, "Администратор"},
    {5, "Старший администратор"},
    {5, "Младший тестировщик"},
    {5, "Тестировщик"},
    {6, "Следящий за Правительством"},
    {6, "Следящий за УМВД"},
    {6, "Следящий за ФСБ"},
    {6, "Следящий за ЦБ"},
    {6, "Следящий за СМИ"},
    {6, "Следящий за ВЧ"},
    {6, "Следящий за МВД"},
    {6, "Следящий за ФСИН"},
    {6, "Следящий за А-ОПГ"},
    {6, "Следящий за Б-ОПГ"},
    {6, "Следящий за Л-ОПГ"},
    {7, "ГС ГОСС"},
    {7, "ЗГС ГОСС"},
    {7, "ГС ОПГ"},
    {7, "ЗГС ОПГ"},
    {7, "Старший тестировщик"},
    {7, "Контроль качества"},
    {8, "Куратор администрации"},
    {8, "Зам. куратора АП"},
    {8, "Куратор АП"},
    {8, "Отдел кадров"},
    {8, "QA"},
    {9, "Зам. Главного администратора"},
    {9, "Осн. Зам. Главного администратора"},
    {9, "Технический специалист"},
    {9, "Зам. куратора тех. специалистов"},
    {9, "Куратор тех. специалистов"},
    {9, "Руководитель контроля качества"},
    {9, "Руководитель отдела кадров"},
    {9, "Зам. Главного тестировщика"},
    {10, "Главный администратор"},
    {10, "Зам. Главного тех. Администратора"},
    {10, "Главный тех. Администратор"},
    {10, "Зам. Специального администратора"},
    {10, "Специальный администратор"},
    {10, "Главный тестировщик"},
    {11, "Маркетинг"},
    {11, "Команда проекта"},
    {12, "Руководитель проекта"},
    {12, "Директор проекта"},
    {13, "Геймдизайнер"},
    {13, "Разработчик игры"},
    {13, "Владелец проекта"}
};

// Сколько должностей на уровне.
stock SRSet_PosCount(level)
{
    new count;
    for(new i; i < SR_ADMIN_POS_COUNT; i++)
        if(g_SRAdminPos[i][SRP_LEVEL] == level) count++;
    return count;
}

// n-я должность уровня -> индекс в g_SRAdminPos (или -1).
stock SRSet_PosByLevel(level, n)
{
    if(n < 0) return -1;
    new seen;
    for(new i; i < SR_ADMIN_POS_COUNT; i++)
    {
        if(g_SRAdminPos[i][SRP_LEVEL] != level) continue;
        if(seen == n) return i;
        seen++;
    }
    return -1;
}

// Название должности игрока (для /admins и т.п.). Если должность не выбрана или не подходит к уровню - название уровня.
stock SRSet_RankNameFor(playerid, dest[], size)
{
    new level = SRAdmin_Level(playerid), pos = g_SRAdminPosOf[playerid];
    if(level > 0 && pos >= 1 && pos <= SR_ADMIN_POS_COUNT && g_SRAdminPos[pos - 1][SRP_LEVEL] == level)
        format(dest, size, "%s", g_SRAdminPos[pos - 1][SRP_NAME]);
    else
        AdminsList_RankName(level, dest, size);
    return 1;
}

// Сбросить сохранённую должность, если она не принадлежит текущему уровню (после /setadm, /deladmin, снятия по варнам).
stock SRSet_Sanitize(playerid)
{
    if(playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid)) return 0;
    new pos = g_SRAdminPosOf[playerid];
    if(pos == 0) return 1;
    new level = SRAdmin_Level(playerid);
    if(level > 0 && pos >= 1 && pos <= SR_ADMIN_POS_COUNT && g_SRAdminPos[pos - 1][SRP_LEVEL] == level) return 1;
    g_SRAdminPosOf[playerid] = 0;
    new query[96];
    mysql_format(mysql, query, sizeof query, "UPDATE accounts SET admin_pos=0 WHERE id=%d LIMIT 1", GetPlayerAccountID(playerid));
    mysql_query(mysql, query, false);
    if(mysql_errno(mysql)) printf("[ADMIN] POS_CLEAR_FAILED account=%d errno=%d", GetPlayerAccountID(playerid), mysql_errno(mysql));
    return 1;
}

// Применить уровень + должность. Права проверяются повторно (диалог мог устареть).
stock SRSet_Apply(playerid, targetid, level, pos)
{
    if(!SRAdmin_Require(playerid, SR_SETADMIN_MIN_LEVEL)) return 1;
    if(targetid < 0 || targetid >= MAX_PLAYERS || !IsPlayerConnected(targetid) || !IsPlayerLogged(targetid) || targetid == playerid)
        return SendClientMessage(playerid, 0xFF5533FF, "Игрок не найден.");
    new actor_level = SRAdmin_Level(playerid), target_level = SRAdmin_Level(targetid), max_level = SRSet_Max(actor_level);
    if(level < 1 || level > SR_ADMIN_MAX_LEVEL || level > actor_level || level > max_level || target_level > actor_level || target_level > max_level)
        return SendClientMessage(playerid, 0xFF5533FF, "Недостаточно прав для выдачи этого уровня.");
    if(GetPlayerAdminEx(targetid) != 0 && target_level == 0)
        return SendClientMessage(playerid, 0xFF5533FF, "У игрока неизвестный админ-уровень. Изменение запрещено.");
    if(pos < 1 || pos > SR_ADMIN_POS_COUNT || g_SRAdminPos[pos - 1][SRP_LEVEL] != level)
        return SendClientMessage(playerid, 0xFF5533FF, "Некорректная должность для выбранного уровня.");

    new old_level = GetPlayerAdminEx(targetid), old_pos = g_SRAdminPosOf[targetid];
    if(old_level == level && old_pos == pos)
        return SendClientMessage(playerid, -1, "У игрока уже установлена эта должность.");

    new query[384], rank_name[SR_ADMIN_POS_NAME_LEN], prefix[32], account = GetPlayerAccountID(targetid);
    format(rank_name, sizeof rank_name, "%s", g_SRAdminPos[pos - 1][SRP_NAME]);
    format(prefix, sizeof prefix, "%s", rank_name);

    // Критичное сохранение: уровень и должность.
    mysql_format(mysql, query, sizeof query,
        "UPDATE accounts SET admin=%d,admin_pos=%d WHERE id=%d AND admin=%d LIMIT 1", level, pos, account, old_level);
    new Cache:result = mysql_query(mysql, query);
    new error = mysql_errno(mysql);
    new affected = error ? 0 : cache_affected_rows(mysql);
    if(result != Cache:0) cache_delete(result, mysql);
    if(error || affected != 1)
    {
        printf("[ADMIN] LEVEL_SAVE_FAILED actor=%d target=%d account=%d errno=%d affected=%d", playerid, targetid, account, error, affected);
        return SendClientMessage(playerid, 0xFF5533FF, "Не удалось сохранить должность. Изменения в игре не применены.");
    }
    // Префикс - второй запрос: если колонка prefix короче названия, выдача всё равно состоялась.
    mysql_format(mysql, query, sizeof query, "UPDATE accounts SET prefix='%e' WHERE id=%d LIMIT 1", prefix, account);
    mysql_query(mysql, query, false);
    if(mysql_errno(mysql)) printf("[ADMIN] PREFIX_SAVE_FAILED target_account=%d errno=%d", account, mysql_errno(mysql));

    // End observation while the old permission is still available to its cleanup function.
    if(GetPlayerSpectateData(targetid, S_PLAYER) != -1) StopSpectate(targetid);
    SetPlayerData(targetid, P_ADMIN, level);
    g_SRAdminPosOf[targetid] = pos;
    SetPlayerData(targetid, P_ADMIN_LOGGED, 0);
    SetPlayerColorInit(targetid);
    format(GetPlayerData(targetid, P_PREFIX), 32, "%s", prefix);
    SRAdmin_ClearTarget(targetid);
    DeletePVar(targetid, "SRAdmPinStep");

    new message[192];
    format(message, sizeof message, "[A] %s[%d] изменил должность %s[%d]: %d -> %d ур. (%s)",
        GetPlayerNameEx(playerid), playerid, GetPlayerNameEx(targetid), targetid, old_level, level, rank_name);
    SendMessageToAdmins(message, 0xFF5533FF);
    SendClientMessage(targetid, 0xFF5533FF, message);
    printf("[ADMIN] LEVEL_CHANGED actor=%d actor_account=%d target=%d target_account=%d old=%d new=%d pos=%d",
        playerid, GetPlayerAccountID(playerid), targetid, account, old_level, level, pos);

    new description[160], log_query[512];
    format(description, sizeof description, "Admin level: target=%s account=%d old=%d new=%d pos=%d (%s)",
        GetPlayerNameEx(targetid), account, old_level, level, pos, rank_name);
    mysql_format(mysql, log_query, sizeof log_query,
        "INSERT INTO action_log (acc_id,uip,type,description,time) VALUES (%d,'%e',%d,'%e',%d)",
        GetPlayerAccountID(playerid), GetPlayerIpEx(playerid), LOG_TYPE_SET_ADMIN, description, gettime());
    mysql_query(mysql, log_query, false);
    if(mysql_errno(mysql)) printf("[ADMIN] LEVEL_LOG_FAILED actor_account=%d target_account=%d errno=%d", GetPlayerAccountID(playerid), account, mysql_errno(mysql));
    SRAdmin_ShowLogin(targetid);
    return 1;
}

// ---------------------------------------------------------------------------------------------
// Окна /setadmin: 1) блоки уровней, 2) должности уровня (+ пункт "Команды уровня"), 3) справка по командам уровня.
// ---------------------------------------------------------------------------------------------

stock SRSet_Clear(playerid)
{
    SRAdmin_ClearTarget(playerid);
    DeletePVar(playerid, "SRAdmLevel");
    return 1;
}

// Описание команд уровня для окна после нажатия на уровень.
stock SRSet_LevelCommands(level, dest[], size)
{
    dest[0] = EOS;
    switch(level)
    {
        case 1: strcat(dest, "/mute, /jail, /af, /sp, /spoff, /askin, /a, /rep, /astats", size);
        case 2: strcat(dest, "/kick, /unmute, /unjail", size);
        case 3: strcat(dest, "/ban, /offban, /offjail, /warn, /offwarn, /slap, /gh (/gethere), /spawn, /veh, /delveh, /sethp, /skin (выдать временный скин)", size);
        case 4: strcat(dest, "/permban, /offpermban, /unban, /unwarn, /givemet, /skick (скрытый кик)", size);
        case 5:
        {
            strcat(dest, "/cc\n", size);
            strcat(dest, "/tskin (только для должностей: Младший тестировщик, Тестировщик, Старший тестировщик, Контроль качества, Отдел кадров, QA, Руководитель контроля качества, Руководитель отдела кадров, Зам. Главного тестировщика, Главный тестировщик)\n", size);
            strcat(dest, "Также: /a, /ban, /offban, /offjail, /warn, /offwarn, /slap, /gh (/gethere), /spawn, /veh, /delveh, /sethp, /skin (выдать временный скин), /kick, /unmute, /unjail, /mute, /jail, /af, /sp, /spoff, /askin, /rep, /astats", size);
        }
        case 6: strcat(dest, "/invite (принять игрока в любую фракцию), /uninvite (уволить игрока с любой фракции), /giverank (повысить или понизить ранг игрока в организации; нельзя трогать лидера и зама лидера), /setleader (выдать лидерку игроку)", size);
        case 7:
        {
            strcat(dest, "/setadmin (можно выдать только 6 уровень)\n", size);
            strcat(dest, "/giveitem, /giveepoints, /skin (эти 3 команды работают при включённом Вайт-листе)\n", size);
            strcat(dest, "/tchat (чат тестеров)", size);
        }
        case 8: strcat(dest, "/setleader, /setadmin, /sethelper, /aban (заблокировать доступ к админ-панели другому администратору от слива админки; не работает для тех, кто выше по должности)", size);
        case 9:
        {
            strcat(dest, "/tradelog, /checkinv, /getlog, /changeproplog, /famlog, /smslog, /chatlog, /banklog, /bylog\n", size);
            strcat(dest, "/tdonate [кол-во] - выдача доната, /tcash [кол-во] - выдача ИВ, /bplevel [кол-во уровней] - выдать уровень БП, /scorebp - выдача БП икспа (работают при включённом Вайт-листе)", size);
        }
        case 10:
        {
            strcat(dest, "/tradelog, /checkinv, /getlog, /changeproplog, /famlog, /smslog, /chatlog, /banklog, /bylog\n", size);
            strcat(dest, "/setskin (выдать скин навсегда; только для должностей от Зам. Гл. Тех. Администратора)\n", size);
            strcat(dest, "/sethouseowner (принудительно сменить владельца дома; от должности Гл. Тех. Администратор)\n", size);
            strcat(dest, "/setbizowner (принудительно сменить владельца бизнеса; от должности Гл. Тех. Администратор)\n", size);
            strcat(dest, "/givecash (от должности Зам. Гл. Тех. Администратора)", size);
        }
        case 11, 12, 13: strcat(dest, "Все команды в игре", size);
        default: strcat(dest, "-", size);
    }
    return 1;
}

stock SRSet_TargetStillValid(playerid)
{
    new targetid = GetPVarInt(playerid, "SRAdmTarget");
    if(targetid < 0 || targetid >= MAX_PLAYERS || !IsPlayerConnected(targetid) || !IsPlayerLogged(targetid)) return 0;
    if(GetPlayerAccountID(targetid) != GetPVarInt(playerid, "SRAdmTargetAccount")) return 0;
    if(g_SRAdminSession[targetid] != GetPVarInt(playerid, "SRAdmTargetSession")) return 0;
    return 1;
}

// Общая проверка перед каждым шагом окон: права выдающего + цель не поменялась.
stock SRSet_Valid(playerid)
{
    if(!SRAdmin_Require(playerid, SR_SETADMIN_MIN_LEVEL))
    {
        SRSet_Clear(playerid);
        return 0;
    }
    if(!SRSet_TargetStillValid(playerid))
    {
        SRSet_Clear(playerid);
        SendClientMessage(playerid, 0xFF5533FF, "Игрок вышел или изменился. Откройте /setadmin заново.");
        return 0;
    }
    return 1;
}

stock SRSet_ShowLevels(playerid)
{
    new targetid = GetPVarInt(playerid, "SRAdmTarget");
    new max_level = SRSet_Max(SRAdmin_Level(playerid)), current = SRAdmin_Level(targetid);
    new list[1536], row[128], title[64];
    format(list, sizeof list, "Уровень\tДолжности");
    for(new level = 1; level <= SR_ADMIN_MAX_LEVEL; level++)
    {
        if(level > max_level)
            format(row, sizeof row, "\n{777777}%d уровень\t{777777}недоступно", level);
        else
            format(row, sizeof row, "\n%d уровень\tДолжностей: %d%s", level, SRSet_PosCount(level), (level == current) ? " {33CC33}(сейчас)" : "");
        strcat(list, row, sizeof list);
    }
    format(title, sizeof title, "Выдача админки: %s[%d]", GetPlayerNameEx(targetid), targetid);
    SetPVarInt(playerid, "SRAdmTargetPending", 1);
    DeletePVar(playerid, "SRAdmLevel");
    return Dialog(playerid, DIALOG_SETADMINS, DIALOG_STYLE_TABLIST_HEADERS, title, list, "Выбрать", "Закрыть");
}

stock SRSet_ShowPositions(playerid, level)
{
    new targetid = GetPVarInt(playerid, "SRAdmTarget");
    new count = SRSet_PosCount(level), current_pos = g_SRAdminPosOf[targetid];
    new list[1536], row[96], title[64];
    for(new n; n < count; n++)
    {
        new idx = SRSet_PosByLevel(level, n);
        if(idx < 0) continue;
        format(row, sizeof row, "%s%s%s", n ? "\n" : "", g_SRAdminPos[idx][SRP_NAME],
            (SRAdmin_Level(targetid) == level && current_pos == idx + 1) ? " {33CC33}(сейчас)" : "");
        strcat(list, row, sizeof list);
    }
    // Последний пункт - справка по командам уровня (не назначает должность).
    strcat(list, "\n{AAAAAA}Команды уровня", sizeof list);
    format(title, sizeof title, "%d уровень: должность", level);
    SetPVarInt(playerid, "SRAdmTargetPending", 2);
    SetPVarInt(playerid, "SRAdmLevel", level);
    return Dialog(playerid, DIALOG_SETADMIN_POS, DIALOG_STYLE_LIST, title, list, "Выбрать", "Назад");
}

stock SRSet_ShowLevelInfo(playerid, level)
{
    new text[2048], names[768], commands[1536], title[64];
    for(new n, count = SRSet_PosCount(level); n < count; n++)
    {
        new idx = SRSet_PosByLevel(level, n);
        if(idx < 0) continue;
        if(n) strcat(names, ", ", sizeof names);
        strcat(names, g_SRAdminPos[idx][SRP_NAME], sizeof names);
    }
    SRSet_LevelCommands(level, commands, sizeof commands);
    format(text, sizeof text, "{FFCD00}Должности:\n{FFFFFF}%s\n\n{FFCD00}Команды%s:\n{FFFFFF}%s",
        names, (level > 1) ? " (дополняют команды предыдущих уровней)" : "", commands);
    format(title, sizeof title, "%d уровень: команды", level);
    SetPVarInt(playerid, "SRAdmTargetPending", 3);
    SetPVarInt(playerid, "SRAdmLevel", level);
    return Dialog(playerid, DIALOG_SETADMIN_INFO, DIALOG_STYLE_MSGBOX, title, text, "Назад", "");
}

stock SRSet_Open(playerid, targetid)
{
    SRSet_Clear(playerid);
    if(!SRAdmin_Require(playerid, SR_SETADMIN_MIN_LEVEL)) return 1;
    if(targetid < 0 || targetid >= MAX_PLAYERS || !IsPlayerConnected(targetid) || !IsPlayerLogged(targetid) || targetid == playerid)
        return SendClientMessage(playerid, 0xFF5533FF, "Некорректный игрок.");
    if(SRAdmin_Level(targetid) > SRSet_Max(SRAdmin_Level(playerid)))
        return SendClientMessage(playerid, 0xFF5533FF, "У игрока уровень выше, чем вы можете изменить.");
    SetPVarInt(playerid, "SRAdmTarget", targetid);
    SetPVarInt(playerid, "SRAdmTargetAccount", GetPlayerAccountID(targetid));
    SetPVarInt(playerid, "SRAdmTargetSession", g_SRAdminSession[targetid]);
    return SRSet_ShowLevels(playerid);
}

stock SRSet_Response(playerid, dialogid, response, listitem)
{
    new expected;
    if(dialogid == DIALOG_SETADMINS) expected = 1;
    else if(dialogid == DIALOG_SETADMIN_POS) expected = 2;
    else if(dialogid == DIALOG_SETADMIN_INFO) expected = 3;
    if(!expected || GetPVarInt(playerid, "SRAdmTargetPending") != expected) return 1;
    if(!SRSet_Valid(playerid)) return 1;

    new max_level = SRSet_Max(SRAdmin_Level(playerid)), targetid = GetPVarInt(playerid, "SRAdmTarget");
    if(SRAdmin_Level(targetid) > max_level)
    {
        SRSet_Clear(playerid);
        return SendClientMessage(playerid, 0xFF5533FF, "У игрока уровень выше, чем вы можете изменить.");
    }

    if(expected == 1)
    {
        if(!response)
        {
            SRSet_Clear(playerid);
            return 1;
        }
        new level = listitem + 1;
        if(level < 1 || level > SR_ADMIN_MAX_LEVEL) return SRSet_ShowLevels(playerid);
        if(level > max_level)
        {
            SendClientMessage(playerid, 0xFF5533FF, "Этот уровень вам недоступен.");
            return SRSet_ShowLevels(playerid);
        }
        return SRSet_ShowPositions(playerid, level);
    }

    new level = GetPVarInt(playerid, "SRAdmLevel");
    if(level < 1 || level > SR_ADMIN_MAX_LEVEL || level > max_level) return SRSet_ShowLevels(playerid);

    if(expected == 3) return SRSet_ShowPositions(playerid, level);

    // expected == 2: окно должностей
    if(!response) return SRSet_ShowLevels(playerid);
    new count = SRSet_PosCount(level);
    if(listitem == count) return SRSet_ShowLevelInfo(playerid, level);
    new idx = SRSet_PosByLevel(level, listitem);
    if(listitem < 0 || idx < 0) return SRSet_ShowPositions(playerid, level);
    SRSet_Clear(playerid);
    return SRSet_Apply(playerid, targetid, level, idx + 1);
}
