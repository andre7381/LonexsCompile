// admin_skarf.inc - все административные обновления в одном файле.
// Порядок: core, menu, help, bans, af, af_quick, helper, setadmin. Подключается из icerussia.pwn одной строкой.

// ======================================================================
// core
// ======================================================================
#define SRAdmin_PinResponse SRAdmin_PinResponse_Legacy
// Administrative identity and PIN flow adapted from skarf/ryzen.
// This is a staging module, not the completed administrative-system replacement.
#if defined _sr_admin_core_included
    #endinput
#endif
#define _sr_admin_core_included

#define SR_ADMIN_MAX_LEVEL (13)

new g_SRAdminSession[MAX_PLAYERS];
new const g_SRAdminRank[][32] =
{
    "Игрок", "Младший модератор", "Модератор", "Старший модератор",
    "Администратор", "Старший администратор", "ГС/ЗГС",
    "Куратор администрации", "Технический специалист",
    "Зам. главного администратора", "Главный администратор",
    "Команда проекта", "Заместитель основателя", "Основатель"
};

stock SRAdmin_Level(playerid)
{
    if(playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid)) return 0;
    new level = GetPlayerAdminEx(playerid);
    // v57 has a named founder role at level 14. No other unknown levels are granted access.
    if(level == 14) return SR_ADMIN_MAX_LEVEL;
    if(level < 0 || level > SR_ADMIN_MAX_LEVEL) return 0;
    return level;
}

stock SRAdmin_Session(playerid)
{
    if(playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid)) return -1;
    return g_SRAdminSession[playerid];
}

stock SRAdmin_ClearTarget(playerid)
{
    DeletePVar(playerid, "SRAdmTarget");
    DeletePVar(playerid, "SRAdmTargetAccount");
    DeletePVar(playerid, "SRAdmTargetSession");
    DeletePVar(playerid, "SRAdmTargetPending");
    return 1;
}

stock SRAdmin_Reset(playerid)
{
    g_SRAdminSession[playerid]++;
    SRAdmin_ClearTarget(playerid);
    DeletePVar(playerid, "SRAdmPinStep");
    DeletePVar(playerid, "SRAdmPinAttempts");
    DeletePVar(playerid, "SRAdmPinBlocked");
    DeletePVar(playerid, "AdminLoginAttempts");
    DeletePVar(playerid, "AdminPass");
    DeletePVar(playerid, "SRTeleportPending");
    DeletePVar(playerid, "SRAdminsPending");
    DeletePVar(playerid, "SRAdminsPage");
    DeletePVar(playerid, "SRAdminsPrevious");
    DeletePVar(playerid, "SRAdminsNext");
    DeletePVar(playerid, "SRSpecTargetSession");
    DeletePVar(playerid, "SRSpecStartHouse");
    DeletePVar(playerid, "SRSpecStartBiz");
    DeletePVar(playerid, "SRHelpPending");
    DeletePVar(playerid, "SRHelpLevel");
    DeletePVar(playerid, "SRHelpPage");
    DeletePVar(playerid, "SRHelpMore");
    SetPlayerData(playerid, P_ADMIN_LOGGED, 0);
    return 1;
}

stock SRAdmin_ShowLogin(playerid)
{
    if(!IsPlayerLogged(playerid) || !SRAdmin_Level(playerid)) return 0;
    if(GetPlayerData(playerid, P_ADMIN_LOGGED)) return 1;
    if(GetPVarInt(playerid, "SRAdmPinBlocked")) return 0;
    new registration = GetPlayerData(playerid, P_APASS) == 0;
    SetPVarInt(playerid, "SRAdmPinStep", registration ? 1 : 2);
    if(registration)
        return Dialog(playerid, DIALOG_ADMIN_REG, DIALOG_STYLE_PASSWORD,
            "Регистрация админ-панели", "Придумайте отдельный пароль администратора: 4 цифры, от 1000 до 9999.", "Готово", "Отмена");
    return Dialog(playerid, DIALOG_ADMIN_LOGIN, DIALOG_STYLE_PASSWORD,
        "Вход в админ-панель", "Введите свой административный пароль.", "Войти", "Отмена");
}

stock SRAdmin_Require(playerid, min_level)
{
    if(!IsPlayerLogged(playerid) || SRAdmin_Level(playerid) < min_level)
    {
        SendClientMessage(playerid, 0xFF5533FF, "Недостаточно административных прав.");
        return 0;
    }
    if(!GetPlayerData(playerid, P_ADMIN_LOGGED))
    {
        SRAdmin_ShowLogin(playerid);
        return 0;
    }
    return 1;
}

// A-LOGIN: IP видны админам 1-7 уровня, у 8+ пишется СКРЫТО
#define SR_ALOGIN_COLOR      (0x33CC33FF)
#define SR_ALOGIN_C_MAIN     "{33CC33}"   // основной цвет текста (как SR_ALOGIN_COLOR)
#define SR_ALOGIN_C_DARK     "{1E7A1E}"   // тёмный: должность, ник [id], префикс, IP
#define SR_ALOGIN_IP_MAX_LVL (7)

stock SRAdmin_SendLoginInfo(playerid, level)
{
    new msg[160];

    // 1) Лично админу
    format(msg, sizeof msg, "[A-LOGIN]: Вы авторизовались как: "SR_ALOGIN_C_DARK"%s", g_SRAdminRank[level]);
    SendClientMessage(playerid, SR_ALOGIN_COLOR, msg);

    // 2) Всем админам онлайн: ник [id] и префикс
    format(msg, sizeof msg, "[A-LOGIN]: "SR_ALOGIN_C_DARK"%s [%d] "SR_ALOGIN_C_MAIN"авторизовался как: "SR_ALOGIN_C_DARK"%s", GetPlayerNameEx(playerid), playerid, GetPlayerData(playerid, P_PREFIX));
    SendMessageToAdmins(msg, SR_ALOGIN_COLOR);

    // 3) Всем админам онлайн: IP или СКРЫТО
    if(level <= SR_ALOGIN_IP_MAX_LVL)
    {
        format(msg, sizeof msg, "[A-LOGIN]: IP: "SR_ALOGIN_C_DARK"%s"SR_ALOGIN_C_MAIN" | REG-IP: "SR_ALOGIN_C_DARK"%s"SR_ALOGIN_C_MAIN" | LAST-IP: "SR_ALOGIN_C_DARK"%s",
            GetPlayerIpEx(playerid), GetPlayerData(playerid, P_REG_IP), GetPlayerData(playerid, P_LAST_IP));
    }
    else
    {
        format(msg, sizeof msg, "[A-LOGIN]: IP: "SR_ALOGIN_C_DARK"СКРЫТО"SR_ALOGIN_C_MAIN" | REG-IP: "SR_ALOGIN_C_DARK"СКРЫТО"SR_ALOGIN_C_MAIN" | LAST-IP: "SR_ALOGIN_C_DARK"СКРЫТО");
    }
    SendMessageToAdmins(msg, SR_ALOGIN_COLOR);
    return 1;
}

stock SRAdmin_PinResponse(playerid, dialogid, response, const inputtext[])
{
    if(!IsPlayerLogged(playerid) || !SRAdmin_Level(playerid)) return 1;
    if(GetPlayerData(playerid, P_ADMIN_LOGGED) || GetPVarInt(playerid, "SRAdmPinBlocked")) return 1;
    new expected = (dialogid == DIALOG_ADMIN_REG) ? 1 : 2;
    if(GetPVarInt(playerid, "SRAdmPinStep") != expected) return 1;
    DeletePVar(playerid, "SRAdmPinStep");
    if(!response) return 1;
    new length = strlen(inputtext), numeric = length == 4;
    for(new i; i < length; i++)
        if(inputtext[i] < '0' || inputtext[i] > '9') numeric = 0;

    if(expected == 1)
    {
        if(GetPlayerData(playerid, P_APASS) != 0) return 1;
        if(!numeric || length != 4 || strval(inputtext) < 1000)
        {
            SendClientMessage(playerid, 0xFF5533FF, "Пароль должен состоять из четырёх цифр, от 1000 до 9999.");
            return SRAdmin_ShowLogin(playerid);
        }
        new query[192];
        mysql_format(mysql, query, sizeof query,
            "UPDATE accounts SET AdminPass=%d WHERE id=%d AND AdminPass=0 AND admin=%d LIMIT 1",
            strval(inputtext), GetPlayerAccountID(playerid), GetPlayerAdminEx(playerid));
        new Cache:result = mysql_query(mysql, query);
        new error = mysql_errno(mysql);
        new affected = error ? 0 : cache_affected_rows(mysql);
        if(result != Cache:0) cache_delete(result, mysql);
        if(error || affected != 1)
        {
            printf("[ADMIN] PIN_SAVE_FAILED player=%d account=%d errno=%d affected=%d", playerid, GetPlayerAccountID(playerid), error, affected);
            SendClientMessage(playerid, 0xFF5533FF, "Не удалось сохранить административный пароль. Повторите вход позже.");
            return 1;
        }
        SetPlayerData(playerid, P_APASS, strval(inputtext));
        return SRAdmin_ShowLogin(playerid);
    }

    if(!numeric || GetPlayerData(playerid, P_APASS) == 0 || strval(inputtext) != GetPlayerData(playerid, P_APASS))
    {
        new attempts = GetPVarInt(playerid, "SRAdmPinAttempts") + 1;
        SetPVarInt(playerid, "SRAdmPinAttempts", attempts);
        printf("[ADMIN] LOGIN_FAILED player=%d account=%d attempt=%d", playerid, GetPlayerAccountID(playerid), attempts);
        if(attempts >= 3)
        {
            SetPVarInt(playerid, "SRAdmPinBlocked", 1);
            return FixKick(playerid);
        }
        SendClientMessage(playerid, 0xFF5533FF, "Неверный административный пароль.");
        return SRAdmin_ShowLogin(playerid);
    }

    DeletePVar(playerid, "SRAdmPinAttempts");
    SetPlayerData(playerid, P_ADMIN_LOGGED, 1);
    RefreshAdminBadge(playerid);
    new level = SRAdmin_Level(playerid);
    SRAdmin_SendLoginInfo(playerid, level);
    printf("[ADMIN] LOGIN_SUCCESS player=%d account=%d level=%d", playerid, GetPlayerAccountID(playerid), level);
    return 1;
}

stock SRAdmin_ApplyLevel(playerid, targetid, level, min_level, strict_hierarchy = 0)
{
    if(!SRAdmin_Require(playerid, min_level)) return 1;
    if(targetid < 0 || targetid >= MAX_PLAYERS || !IsPlayerConnected(targetid) || !IsPlayerLogged(targetid) || targetid == playerid)
        return SendClientMessage(playerid, 0xFF5533FF, "Недопустимый игрок.");
    new actor_level = SRAdmin_Level(playerid), target_level = SRAdmin_Level(targetid);
    if(level < 0 || level > SR_ADMIN_MAX_LEVEL || level > actor_level || target_level > actor_level || (strict_hierarchy && target_level >= actor_level))
        return SendClientMessage(playerid, 0xFF5533FF, "Недопустимый уровень или нарушение иерархии администрации.");
    if(GetPlayerAdminEx(targetid) != 0 && target_level == 0)
        return SendClientMessage(playerid, 0xFF5533FF, "Неизвестный текущий уровень цели. Требуется проверка владельцем.");
    new old_level = GetPlayerAdminEx(targetid);
    if(old_level == level) return SendClientMessage(playerid, -1, "У игрока уже установлен этот уровень.");
    new query[320], rank_name[32];
    if(level > 0) format(rank_name, sizeof rank_name, "%s", g_SRAdminRank[level]);
    mysql_format(mysql, query, sizeof query,
        "UPDATE accounts SET admin=%d,prefix='%e' WHERE id=%d AND admin=%d LIMIT 1",
        level, rank_name, GetPlayerAccountID(targetid), old_level);
    new Cache:result = mysql_query(mysql, query);
    new error = mysql_errno(mysql);
    new affected = error ? 0 : cache_affected_rows(mysql);
    if(result != Cache:0) cache_delete(result, mysql);
    if(error || affected != 1)
    {
        printf("[ADMIN] LEVEL_SAVE_FAILED actor=%d target=%d account=%d errno=%d affected=%d", playerid, targetid, GetPlayerAccountID(targetid), error, affected);
        return SendClientMessage(playerid, 0xFF5533FF, "Не удалось сохранить уровень. Изменение в игре не применено.");
    }
    // End observation while the old permission is still available to its cleanup function.
    if(GetPlayerSpectateData(targetid, S_PLAYER) != -1) StopSpectate(targetid);
    SetPlayerData(targetid, P_ADMIN, level);
    SetPlayerData(targetid, P_ADMIN_LOGGED, 0);
    SetPlayerColorInit(targetid);
    format(GetPlayerData(targetid, P_PREFIX), 32, "%s", rank_name);
    SRAdmin_ClearTarget(targetid);
    DeletePVar(targetid, "SRAdmPinStep");
    new message[192];
    format(message, sizeof message, "[A] %s[%d] изменил уровень %s[%d]: %d -> %d", GetPlayerNameEx(playerid), playerid, GetPlayerNameEx(targetid), targetid, old_level, level);
    SendMessageToAdmins(message, 0xFF5533FF);
    SendClientMessage(targetid, 0xFF5533FF, message);
    printf("[ADMIN] LEVEL_CHANGED actor=%d actor_account=%d target=%d target_account=%d old=%d new=%d", playerid, GetPlayerAccountID(playerid), targetid, GetPlayerAccountID(targetid), old_level, level);
    new description[144], log_query[512];
    format(description, sizeof description, "Admin level: target=%s account=%d old=%d new=%d", GetPlayerNameEx(targetid), GetPlayerAccountID(targetid), old_level, level);
    mysql_format(mysql, log_query, sizeof log_query,
        "INSERT INTO action_log (acc_id,uip,type,description,time) VALUES (%d,'%e',%d,'%e',%d)",
        GetPlayerAccountID(playerid), GetPlayerIpEx(playerid), LOG_TYPE_SET_ADMIN, description, gettime());
    mysql_query(mysql, log_query, false);
    if(mysql_errno(mysql)) printf("[ADMIN] LEVEL_LOG_FAILED actor_account=%d target_account=%d errno=%d", GetPlayerAccountID(playerid), GetPlayerAccountID(targetid), mysql_errno(mysql));
    if(level > 0) SRAdmin_ShowLogin(targetid);
    return 1;
}

stock SRAdmin_OpenAssignment(playerid, targetid)
{
    SRAdmin_ClearTarget(playerid);
    if(!SRAdmin_Require(playerid, 9)) return 1;
    if(targetid < 0 || targetid >= MAX_PLAYERS || !IsPlayerConnected(targetid) || !IsPlayerLogged(targetid) || targetid == playerid)
        return SendClientMessage(playerid, 0xFF5533FF, "Недопустимый игрок.");
    if(SRAdmin_Level(targetid) > SRAdmin_Level(playerid)) return SendClientMessage(playerid, 0xFF5533FF, "У цели уровень выше вашего.");
    SetPVarInt(playerid, "SRAdmTargetPending", 1);
    SetPVarInt(playerid, "SRAdmTarget", targetid);
    SetPVarInt(playerid, "SRAdmTargetAccount", GetPlayerAccountID(targetid));
    SetPVarInt(playerid, "SRAdmTargetSession", g_SRAdminSession[targetid]);
    new list[640];
    for(new level = 1; level <= SR_ADMIN_MAX_LEVEL; level++)
    {
        strcat(list, g_SRAdminRank[level], sizeof list);
        if(level != SR_ADMIN_MAX_LEVEL) strcat(list, "\n", sizeof list);
    }
    return Dialog(playerid, DIALOG_SETADMINS, DIALOG_STYLE_LIST, "Назначение администратора", list, "Выбрать", "Отмена");
}

stock SRAdmin_AssignmentResponse(playerid, response, listitem)
{
    if(!GetPVarInt(playerid, "SRAdmTargetPending")) return 1;
    new targetid = GetPVarInt(playerid, "SRAdmTarget");
    new account = GetPVarInt(playerid, "SRAdmTargetAccount");
    new session = GetPVarInt(playerid, "SRAdmTargetSession");
    SRAdmin_ClearTarget(playerid);
    if(!response) return 1;
    if(!SRAdmin_Require(playerid, 9)) return 1;
    if(listitem < 0 || listitem >= SR_ADMIN_MAX_LEVEL) return 1;
    if(targetid < 0 || targetid >= MAX_PLAYERS || !IsPlayerConnected(targetid) || !IsPlayerLogged(targetid)) return 1;
    if(GetPlayerAccountID(targetid) != account || g_SRAdminSession[targetid] != session)
        return SendClientMessage(playerid, 0xFF5533FF, "Игрок переподключился. Выберите его заново.");
    return SRAdmin_ApplyLevel(playerid, targetid, listitem + 1, 9);
}
#undef SRAdmin_PinResponse

// ======================================================================
// menu
// ======================================================================
// Donor admin chat/list and teleport flow, adapted to the current server's locations.
#if defined _sr_admin_menu_included
    #endinput
#endif
#define _sr_admin_menu_included

stock SRAdmin_WriteLog(playerid, log_type, text[])
{
    new query[1024];
    mysql_format(mysql, query, sizeof query,
        "INSERT INTO action_log (acc_id,uip,type,description,time) VALUES (%d,'%e',%d,'%e',%d)",
        GetPlayerAccountID(playerid), GetPlayerIpEx(playerid), log_type, text, gettime());
    mysql_query(mysql, query, false);
    if(mysql_errno(mysql))
    {
        printf("[ADMIN] LOG_FAILED player=%d account=%d type=%d errno=%d", playerid, GetPlayerAccountID(playerid), log_type, mysql_errno(mysql));
        return 0;
    }
    return 1;
}

stock SRAdmin_TeleportAllowed(playerid)
{
    if(!IsPlayerLogged(playerid)) return 0;
    // Donor permits YouTube staff as well. An account that is also an admin must enter its PIN.
    if(GetPlayerAdminEx(playerid) > 0) return SRAdmin_Require(playerid, 1);
    if(GetPlayerYouTubeEx(playerid) > 0) return 1;
    SendClientMessage(playerid, 0xFF5533FF, "Недостаточно прав для телепортации.");
    return 0;
}

stock SRAdmin_ViewerAuthorized(playerid)
{
    if(playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid) || !IsPlayerLogged(playerid)) return 0;
    if(GetPlayerAdminEx(playerid) > 0) return SRAdmin_Level(playerid) >= 1 && GetPlayerData(playerid, P_ADMIN_LOGGED);
    return GetPlayerYouTubeEx(playerid) >= 1;
}

stock SRAdmin_ShowTeleports(playerid)
{
    DeletePVar(playerid, "SRTeleportPending");
    if(!SRAdmin_TeleportAllowed(playerid)) return 1;
    new list_text[64 * sizeof(g_teleport_list)], sr_tp_row[64];
    for(new i; i < sizeof(g_teleport_list); i++)
    {
        format(sr_tp_row, sizeof sr_tp_row, "%d. %s\n", i + 1, GetTeleportListData(i, TL_NAME));
        strcat(list_text, sr_tp_row, sizeof list_text);
    }
    SetPVarInt(playerid, "SRTeleportPending", 1);
    return Dialog(playerid, DIALOG_TELEPORT_LIST, DIALOG_STYLE_LIST,
        "Важные места", list_text, "Выбрать", "Назад");
}

stock SRAdmin_TeleportResponse(playerid, response, listitem)
{
    if(!GetPVarInt(playerid, "SRTeleportPending")) return 1;
    DeletePVar(playerid, "SRTeleportPending");
    if(!response) return 1;
    if(!SRAdmin_TeleportAllowed(playerid)) return 1;
    if(listitem < 0 || listitem >= sizeof(g_teleport_list)) return 1;
    // Donor teleports the player, not the vehicle. All listed destinations are in world/interior 0.
    if(GetPlayerSpectateData(playerid, S_PLAYER) != -1) StopSpectate(playerid);
    SetPlayerPosEx(playerid,
        GetTeleportListData(listitem, TL_POS_X), GetTeleportListData(listitem, TL_POS_Y),
        GetTeleportListData(listitem, TL_POS_Z), 0.0, 0, 0, false);
    SetPlayerInHouse(playerid, -1);
    SetPlayerInBiz(playerid, -1);
    TogglePlayerControllable(playerid, true);
    new description[128];
    format(description, sizeof description, "Teleport: index=%d name=%s", listitem, GetTeleportListData(listitem, TL_NAME));
    SRAdmin_WriteLog(playerid, LOG_TYPE_ADMIN_ACTION, description);
    printf("[ADMIN] TELEPORT player=%d account=%d destination=%d", playerid, GetPlayerAccountID(playerid), listitem);
    SendClientMessage(playerid, 0x66CC00FF, "Вы успешно телепортировались.");
    return 1;
}

stock SRAdmin_GetChatRankName(level, dest[], size)
{
    if(size <= 0) return 0;
    dest[0] = EOS;

    switch(level)
    {
        case 1: format(dest, size, "%s", ADM_LVL1_NAME);
        case 2: format(dest, size, "%s", ADM_LVL2_NAME);
        case 3: format(dest, size, "%s", ADM_LVL3_NAME);
        case 4: format(dest, size, "%s", ADM_LVL4_NAME);
        case 5: format(dest, size, "%s", ADM_LVL5_NAME);
        case 6: format(dest, size, "%s", ADM_LVL6_NAME);
        case 7: format(dest, size, "%s", ADM_LVL7_NAME);
        case 8: format(dest, size, "%s", ADM_LVL8_NAME);
        case 9: format(dest, size, "%s", ADM_LVL9_NAME);
        case 10: format(dest, size, "%s", ADM_LVL10_NAME);
        case 11: format(dest, size, "%s", ADM_LVL11_NAME);
        case 12: format(dest, size, "%s", ADM_LVL12_NAME);
        case 13: format(dest, size, "%s", ADM_LVL13_NAME);
        default: return 0;
    }
    return 1;
}

stock SRAdmin_Chat(playerid, params[])
{
    // /a доступен только реальному администратору с валидным уровнем и после входа в админ-панель.
    if(!SRAdmin_Require(playerid, 1)) return 1;

    new admin_level = SRAdmin_Level(playerid);
    if(admin_level < 1 || admin_level > SR_ADMIN_MAX_LEVEL)
        return SendClientMessage(playerid, 0xFF5533FF, "Некорректный уровень администратора. Доступ к /a запрещён."), 1;

    if(!strlen(params)) return SendClientMessage(playerid, -1, "Используйте: /a [текст]");
    // Reject overlong input instead of silently cutting a moderation message.
    if(strlen(params) > 80) return SendClientMessage(playerid, -1, "Сообщение админ-чата: не более 80 символов.");

    new rank_name[48], message[192];
    if(!SRAdmin_GetChatRankName(admin_level, rank_name, sizeof rank_name))
        return SendClientMessage(playerid, 0xFF5533FF, "Не удалось определить должность администратора."), 1;

    // В /a показываем префикс, выданный через /prefix; если он пустой - должность по admin lvl.
    new chat_rank[48];
    if(strlen(GetPlayerData(playerid, P_PREFIX)))
        format(chat_rank, sizeof chat_rank, "%s", GetPlayerData(playerid, P_PREFIX));
    else
        format(chat_rank, sizeof chat_rank, "%s", rank_name);
    format(message, sizeof message, "[%s] %s[%d]: %s", chat_rank, GetPlayerNameEx(playerid), playerid, params);

    foreach(new targetid : Player)
    {
        if(!IsPlayerLogged(targetid)) continue;
        if(!GetPlayerData(targetid, P_ADMIN_LOGGED)) continue;
        new target_level = SRAdmin_Level(targetid);
        if(target_level < 1 || target_level > SR_ADMIN_MAX_LEVEL) continue;
        SendClientMessage(targetid, 0x40E0D0FF, message);
    }

    SRAdmin_WriteLog(playerid, LOG_TYPE_ADMIN_CHAT, message);
    return 1;
}

stock SRAdmin_CanSeeList(playerid)
{
    if(!IsPlayerLogged(playerid)) return 0;
    if(GetPlayerAdminEx(playerid) > 0 && !SRAdmin_Require(playerid, 1)) return 0;
    // This VIP restriction is present in the donor's /admins, including for admin accounts.
    new vip = GetPlayerPremium(playerid);
    if(vip < 1 || vip > 3)
    {
        SendClientMessage(playerid, 0xCECECEFF, "У Вас нет VIP аккаунта.");
        return 0;
    }
    return 1;
}

stock SRAdmin_ShowList(playerid, page)
{
    DeletePVar(playerid, "SRAdminsPending");
    if(!SRAdmin_CanSeeList(playerid)) return 1;
    if(page < 0 || page > MAX_PLAYERS / 20) return 1;
    new total;
    foreach(new i : Player)
        if(IsPlayerLogged(i) && (SRAdmin_Level(i) > 0 || GetPlayerYouTubeEx(i) > 0)) total++;
    new last_page = total > 0 ? (total - 1) / 20 : 0;
    if(page > last_page) page = last_page;
    new list[4096] = "Никнейм\tДолжность\tИнформация\n", row[160], rank_name[32];
    new position, shown, from = page * 20;
    foreach(new i : Player)
    {
        if(!IsPlayerLogged(i)) continue;
        new level = SRAdmin_Level(i);
        if(!level && GetPlayerYouTubeEx(i) == 0) continue;
        if(position++ < from) continue;
        if(shown == 20) break;
        if(level > 0) format(rank_name, sizeof rank_name, "%s", g_SRAdminRank[level]);
        else format(rank_name, sizeof rank_name, "Ютубер");
        format(row, sizeof row, "%s[%d]\t%s\tв игре\n", GetPlayerNameEx(i), i, rank_name);
        strcat(list, row, sizeof list);
        shown++;
    }
    new previous = -1, next = -1;
    if(total == 0)
    {
        strcat(list, "Администрация не найдена\t-\t-\n", sizeof list);
        shown = 1;
    }
    if(page > 0)
    {
        previous = shown++;
        strcat(list, "Предыдущая страница\t\t\n", sizeof list);
    }
    if(page < last_page)
    {
        next = shown;
        strcat(list, "Следующая страница\t\t\n", sizeof list);
    }
    SetPVarInt(playerid, "SRAdminsPage", page);
    SetPVarInt(playerid, "SRAdminsPrevious", previous);
    SetPVarInt(playerid, "SRAdminsNext", next);
    SetPVarInt(playerid, "SRAdminsPending", 1);
    new title[64];
    format(title, sizeof title, "Администрация в сети (стр. %d)", page + 1);
    return Dialog(playerid, DIALOG_ADMINS, DIALOG_STYLE_TABLIST_HEADERS, title, list, "Выбрать", "Закрыть");
}

stock SRAdmin_ListResponse(playerid, response, listitem)
{
    if(!GetPVarInt(playerid, "SRAdminsPending")) return 0;
    DeletePVar(playerid, "SRAdminsPending");
    if(!response) return 1;
    if(!SRAdmin_CanSeeList(playerid)) return 1;
    if(listitem < 0) return 1;
    new page = GetPVarInt(playerid, "SRAdminsPage");
    if(listitem == GetPVarInt(playerid, "SRAdminsPrevious")) SRAdmin_ShowList(playerid, page - 1);
    else if(listitem == GetPVarInt(playerid, "SRAdminsNext")) SRAdmin_ShowList(playerid, page + 1);
    return 1;
}

stock SRAdmin_TeleportTarget(playerid, targetid)
{
    if(!SRAdmin_TeleportAllowed(playerid)) return 0;
    if(targetid < 0 || targetid >= MAX_PLAYERS || targetid == playerid || !IsPlayerConnected(targetid) || !IsPlayerLogged(targetid))
    {
        SendClientMessage(playerid, -1, "Укажите другого игрока, который уже вошёл в игру.");
        return 0;
    }
    if(GetPlayerSecretEx(targetid) >= 1)
    {
        SendClientMessage(playerid, -1, "У игрока включена привилегия скрытности.");
        return 0;
    }
    return 1;
}

// Internal movement primitive; commands/offer callbacks validate rights before calling it.
stock SRAdmin_MoveNear(actorid, movingid, anchorid)
{
    if(GetPlayerSpectateData(movingid, S_PLAYER) != -1) StopSpectate(movingid);
    new Float:x, Float:y, Float:z;
    GetPlayerPos(anchorid, x, y, z);
    SetPlayerPosEx(movingid, x + 1.0, y + 1.0, z, 0.0, GetPlayerInterior(anchorid), GetPlayerVirtualWorld(anchorid), false);
    SetPlayerInHouse(movingid, GetPlayerInHouse(anchorid));
    SetPlayerInBiz(movingid, GetPlayerInBiz(anchorid));
    TogglePlayerControllable(movingid, true);
    new message[192];
    format(message, sizeof message, "[A] %s[%d] телепортировал %s[%d] к %s[%d]", GetPlayerNameEx(actorid), actorid, GetPlayerNameEx(movingid), movingid, GetPlayerNameEx(anchorid), anchorid);
    SendMessageToAdmins(message, 0x999999FF);
    SRAdmin_WriteLog(actorid, LOG_TYPE_ADMIN_ACTION, message);
    SendClientMessage(actorid, 0x66CC00FF, "Телепортация выполнена.");
    if(movingid != actorid) SendClientMessage(movingid, 0x66CC00FF, "Вы телепортированы администратором.");
    else SendClientMessage(anchorid, 0x66CC00FF, "К вам телепортировался администратор.");
    printf("[ADMIN] TELEPORT actor=%d account=%d moving=%d anchor=%d", actorid, GetPlayerAccountID(actorid), movingid, anchorid);
    return 1;
}

// ======================================================================
// help
// ======================================================================
// Read-only help for the commands present in this staged version. Does not grant rights.
#if defined _sr_admin_help_included
    #endinput
#endif
#define _sr_admin_help_included

enum E_SR_HELP { SRH_LEVEL, SRH_COMMAND[24], SRH_DESCRIPTION[120] };
new const g_SRHelp[][E_SR_HELP] =
{
    {1, "ahelp", "Открыть справку по уровням администрации"},
    {1, "a", "Написать в административный чат"},
    {1, "admins", "Список администрации онлайн; дополнительно требуется VIP 1-3"},
    {1, "astats", "Посмотреть административную статистику"},
    {1, "ans", "Взять жалобу из очереди и ответить игроку"},
    {1, "rep", "Открыть ответ на жалобу, аналог /ans"},
    {1, "sp", "Начать наблюдение за игроком"},
    {1, "spoff", "Завершить наблюдение и вернуться обратно"},
    {1, "tp", "Открыть список мест для телепортации"},
    {1, "tplist", "Открыть меню телепортов, аналог /tp"},
    {1, "goto", "Телепортироваться к игроку без подтверждения; сокращение /g"},
    {1, "gethere", "Телепортировать игрока к себе; нельзя перемещать старшего админа; /gh"},
    {1, "kick", "Отключить игрока от сервера с причиной"},
    {1, "skick", "Отключить игрока без публичного объявления"},
    {1, "mute", "Заблокировать чат на 1-360 минут"},
    {1, "unmute", "Снять блокировку чата"},
    {1, "jail", "Отправить игрока в административную тюрьму на 1-360 минут"},
    {1, "unjail", "Освободить игрока из административной тюрьмы"},
    {1, "slap", "Поднять или опустить игрока по высоте"},
    {1, "agm", "Включить или выключить административное бессмертие"},
    {1, "askin", "Выбрать административную внешность или вернуть прежнюю"},
    {1, "inter", "Изменить интерьер игрока"},
    {1, "world", "Изменить виртуальный мир игрока"},
    {1, "vw", "Изменить виртуальный мир, аналог /world"},
    {1, "setvw", "Изменить виртуальный мир, аналог /world"},
    {1, "flip", "Поставить перевёрнутый автомобиль на колёса"},
    {1, "hp", "Восстановить здоровье; на первом уровне только себе"},
    {1, "ndrive", "Выдать предусмотренное наказание за NonRP-вождение"},
    {1, "dm", "Выдать предусмотренное наказание за DM"},
    {1, "db", "Выдать предусмотренное наказание за DB"},
    {1, "mg", "Выдать мут за Metagaming"},
    {1, "goss", "Выдать мут за нарушение правил гос. чата"},
    {1, "osk", "Выдать мут за оскорбление"},
    {1, "oskadm", "Выдать мут за оскорбление администрации"},
    {1, "caps", "Выдать мут за CAPS"},
    {1, "sk", "Выдать наказание за Spawn Kill"},
    {2, "weap", "Посмотреть оружие и боезапас игрока"},
    {2, "sethp", "Установить здоровье игрока"},
    {2, "settime", "Установить игровое время"},
    {2, "setweather", "Установить погоду"},
    {2, "event", "Открыть управление мероприятием"},
    {2, "freeze", "Заморозить управление игрока"},
    {2, "unfreeze", "Разморозить управление игрока"},
    {2, "namelist", "Одобрить заявку игрока на смену никнейма"},
    {2, "vtp", "Телепортироваться к транспорту по его ID"},
    {2, "fixcar", "Отремонтировать автомобиль"},
    {2, "warn", "Выдать предупреждение; третье блокирует аккаунт на 10 дней"},
    {2, "unwarn", "Снять все предупреждения игрока"},
    {2, "aspawn", "Открыть административный выбор спавна"},
    {3, "ears", "Включить или выключить административный просмотр SMS"},
    {3, "ban", "Заблокировать аккаунт: до 30 дней, с уровня 5 до 90 дней"},
    {3, "offban", "Заблокировать офлайн-аккаунт по нику на 1-90 дней"},
    {3, "unban", "Снять блокировку аккаунта по нику"},
    {3, "setfuel", "Установить количество топлива в автомобиле"},
    {3, "givegun", "Выдать игровое оружие игроку"},
    {3, "money", "Посмотреть финансовые данные аккаунта"},
    {3, "mp", "Отправить объявление о мероприятии"},
    {3, "pg", "Выдать предусмотренное наказание за Powergaming"},
    {3, "flood", "Выдать мут за флуд"},
    {4, "veh", "Создать административный автомобиль"},
    {4, "delveh", "Удалить административный автомобиль"},
    {4, "house", "Телепортироваться к дому по его номеру"},
    {4, "biz", "Телепортироваться к бизнесу по его номеру"},
    {4, "fuelstation", "Телепортироваться к АЗС по её номеру"},
    {4, "setskin", "Установить внешность игроку"},
    {4, "prefix", "Установить административный префикс"},
    {4, "mat", "Выдать мут за ненормативную лексику"},
    {5, "setleader", "Назначить лидера организации"},
    {5, "setspeed", "Изменить скорость транспорта"},
    {6, "admlist", "Открыть список администрации"},
    {6, "agivelic", "Выдать игроку лицензию"},
    {6, "setskills", "Установить игровые навыки игрока"},
    {6, "createytpromo", "Создать промокод для YouTube-партнёра"},
    {6, "debug", "Посмотреть свой virtual world и interior"},
    {6, "permban", "Выдать предусмотренную командой постоянную блокировку"},
    {6, "offpermban", "Постоянно заблокировать офлайн-аккаунт"},
    {6, "setplayerhouse", "Назначить игроку дом"},
    {6, "setplayerbiz", "Назначить игроку бизнес"},
    {6, "givecoins", "Выдать игровые монеты игроку"},
    {6, "cheat", "Выдать предусмотренное наказание за читы"},
    {6, "ppv", "Выдать наказание за передачу или продажу аккаунта"},
    {6, "bot", "Выдать предусмотренное наказание за использование бота"},
    {7, "templeader", "Назначить временного лидера"},
    {7, "skin", "Установить собственную внешность по ID"},
    {8, "getlog", "Посмотреть логи игрока: чат, семья, банк, организация, админ. действия"},
    {8, "get", "Посмотреть информацию об аккаунте по никнейму: IP, устройство, ОС, блокировка"},
    {8, "getacc", "Посмотреть аккаунт и его транспорт по ID аккаунта"},
    {8, "lip", "Найти аккаунты по IP-адресу"},
    {8, "techinfo", "Посмотреть технические данные аккаунта по никнейму"},
    {8, "arecovery", "Открыть административное восстановление"},
    {9, "setadmin", "Назначить административный уровень через меню, не выше своего"},
    {9, "deladmin", "Снять нижестоящего администратора"},
    {9, "avig", "Выдать административный выговор"},
    {9, "unavig", "Снять административный выговор"},
    {9, "delacc", "Удалить аккаунт; необратимая административная операция"},
    {9, "sliv", "Выдать предусмотренное наказание за вред серверу"},
    {10, "cc", "Очистить общий чат"},
    {11, "giverub", "Выдать донат-валюту игроку"},
    {12, "setstats", "Изменить параметр игрока по коду параметра"},
    {12, "uval", "Уволить игрока из организации"},
    {12, "autosalon", "Изменить наличие и слоты автомобилей в автосалоне"},
    {12, "givecash", "Выдать игровые деньги игроку"},
    {12, "givedonate", "Выдать донат-валюту игроку"},
    {13, "setadm", "Установить административный уровень 0-6 по ID игрока"},
    {13, "restart", "Запустить перезапуск сервера"},
    {13, "msg", "Отправить общее административное сообщение"},
    {13, "reklama", "Отправить серверное рекламное уведомление"},
    {13, "inforestart", "Предупредить игроков о перезапуске"},
    {13, "delvehall", "Удалить административные автомобили"},
    {13, "givevip", "Выдать VIP-статус игроку"},
    {13, "reset_cash", "Обнулить игровые деньги игрока"},
    {13, "reset_donate", "Обнулить донат-валюту игрока"},
    {13, "sendcmd", "Выполнить административно заданную команду"},
    {13, "givecar", "Выдать игроку личный автомобиль"},
    {13, "addhouse", "Создать дом"},
    {13, "addbiz", "Создать бизнес"},
    {13, "addfuelst", "Создать АЗС"},
    {13, "payday", "Запустить расчётный час сервера"},
    {13, "addzone", "Создать территорию"},
    {13, "banip", "Заблокировать IP игрока"},
    {13, "banipoff", "Заблокировать указанный IP-адрес"},
    {13, "unbanip", "Снять блокировку IP-адреса"},
    {13, "getname", "Узнать никнейм по номеру аккаунта в БД"},
    {13, "gzcolor", "Команда управления цветом территории; реализация ещё переносится"},
    {13, "sellproperty", "Сбросить владение имуществом по параметрам команды"},
    {13, "setprods", "Установить запас продукции бизнеса"},
    {13, "fullprods", "Пополнить продукцию всех бизнесов"},
    {13, "setfuels", "Установить запас топлива АЗС"},
    {13, "fullfuels", "Пополнить топливо всех АЗС"}
};

stock SRAdmin_ShowHelp(playerid, level = 0, page = 0)
{
    if(!SRAdmin_Require(playerid, 1)) return 1;
    if(level < 0 || level > 13 || page < 0 || page > sizeof(g_SRHelp) / 12) return 1;
    if(level > SRAdmin_Level(playerid)) return SendClientMessage(playerid, -1, "Этот уровень выше вашего. Откройте доступный раздел.");
    new text[3072], row[160], title[80], more;
    if(level == 0)
    {
        for(new i = 1; i <= 13; i++)
        {
            format(row, sizeof row, "%d. %s%s\n", i, g_SRAdminRank[i], i > SRAdmin_Level(playerid) ? " (недоступно)" : "");
            strcat(text, row, sizeof text);
        }
        format(title, sizeof title, "Команды администрации: уровни");
    }
    else
    {
        new total;
        for(new i; i < sizeof(g_SRHelp); i++) if(g_SRHelp[i][SRH_LEVEL] == level) total++;
        if(page > (total > 0 ? (total - 1) / 12 : 0)) return 1;
        format(text, sizeof text, "Команды, добавляемые на этом уровне. Нижние уровни доступны отдельно.\n\n");
        new position, shown;
        for(new i; i < sizeof(g_SRHelp); i++)
        {
            if(g_SRHelp[i][SRH_LEVEL] != level) continue;
            if(position++ < page * 12) continue;
            if(shown == 12) { more = 1; break; }
            format(row, sizeof row, "/%s - %s\n", g_SRHelp[i][SRH_COMMAND], g_SRHelp[i][SRH_DESCRIPTION]);
            strcat(text, row, sizeof text);
            shown++;
        }
        if(!total) strcat(text, "Новых команд нет. Используйте разделы предыдущих уровней.", sizeof text);
        format(title, sizeof title, "%d. %s | %d", level, g_SRAdminRank[level], page + 1);
    }
    SetPVarInt(playerid, "SRHelpPending", 1);
    SetPVarInt(playerid, "SRHelpLevel", level);
    SetPVarInt(playerid, "SRHelpPage", page);
    SetPVarInt(playerid, "SRHelpMore", more);
    if(!level) return Dialog(playerid, 1098, DIALOG_STYLE_LIST, title, text, "Открыть", "Закрыть");
    return Dialog(playerid, 1098, DIALOG_STYLE_MSGBOX, title, text, more ? "Далее" : "К началу", "Уровни");
}

stock SRAdmin_HelpResponse(playerid, response, listitem)
{
    if(!GetPVarInt(playerid, "SRHelpPending")) return 1;
    DeletePVar(playerid, "SRHelpPending");
    if(!SRAdmin_Require(playerid, 1)) return 1;
    new level = GetPVarInt(playerid, "SRHelpLevel");
    if(!level)
    {
        if(response && listitem >= 0 && listitem < 13) return SRAdmin_ShowHelp(playerid, listitem + 1);
        return 1;
    }
    if(!response) return SRAdmin_ShowHelp(playerid);
    return SRAdmin_ShowHelp(playerid, level, GetPVarInt(playerid, "SRHelpMore") ? GetPVarInt(playerid, "SRHelpPage") + 1 : 0);
}

// ======================================================================
// bans
// ======================================================================
// Account bans use both existing tables. No registration/account loader replacement.
#if defined _sr_admin_bans_included
    #endinput
#endif
#define _sr_admin_bans_included

stock SRAdmin_DBChange(query[], const phase[], &affected)
{
    new Cache:result = mysql_query(mysql, query);
    new error = mysql_errno(mysql);
    affected = error ? 0 : cache_affected_rows(mysql);
    if(result != Cache:0) cache_delete(result, mysql);
    if(!error) return 1;
    printf("[ADMIN] SQL_FAILED phase=%s errno=%d", phase, error);
    return 0;
}

stock SRAdmin_DBExec(query[], const phase[])
{
    new affected;
    return SRAdmin_DBChange(query, phase, affected);
}

stock SRAdmin_BanTransaction()
{
    new Cache:result = mysql_query(mysql,
        "SELECT TABLE_NAME FROM information_schema.TABLES WHERE TABLE_SCHEMA=DATABASE() AND TABLE_NAME IN ('accounts','ban_list','listban') AND ENGINE='InnoDB'");
    new error = mysql_errno(mysql);
    new rows = error ? 0 : cache_get_row_count(mysql);
    if(result != Cache:0) cache_delete(result, mysql);
    if(error || rows != 3)
    {
        printf("[ADMIN] BAN_STORAGE_UNSAFE errno=%d innodb_tables=%d expected=3", error, rows);
        return 0;
    }
    return SRAdmin_DBExec("START TRANSACTION", "ban_begin");
}

stock SRAdmin_LockBanAccounts(playerid, account, minimum)
{
    new query[256], actor_found, target_found, actor_level, target_level, invalid_level;
    mysql_format(mysql, query, sizeof query,
        "SELECT id,admin FROM accounts WHERE id IN (%d,%d) ORDER BY id FOR UPDATE", GetPlayerAccountID(playerid), account);
    new Cache:result = mysql_query(mysql, query);
    new error = mysql_errno(mysql);
    new rows = error ? 0 : cache_get_row_count(mysql);
    for(new i; i < rows; i++)
    {
        new id = cache_get_field_content_int(i, "id", mysql);
        new level = cache_get_field_content_int(i, "admin", mysql);
        if(level == 14) level = 13;
        if(level < 0 || level > 13) invalid_level = 1;
        if(id == GetPlayerAccountID(playerid)) { actor_found = 1; actor_level = level; }
        if(id == account) { target_found = 1; target_level = level; }
    }
    if(result != Cache:0) cache_delete(result, mysql);
    return !error && !invalid_level && actor_found && target_found && actor_level >= minimum && target_level <= actor_level && actor_level == SRAdmin_Level(playerid);
}

stock SRAdmin_BanAccount(playerid, account, days, ip[], reason[])
{
    if(!SRAdmin_Require(playerid, 3) || account <= 0 || days < 1 || days > 90) return 0;
    if(!SRAdmin_BanTransaction()) return 0;
    if(!SRAdmin_LockBanAccounts(playerid, account, 3))
    {
        SRAdmin_DBExec("ROLLBACK", "ban_rights_rollback");
        return 0;
    }
    new result = SRAdmin_WriteBanLocked(playerid, account, days, ip, reason);
    if(result != 1)
    {
        SRAdmin_DBExec("ROLLBACK", "ban_write_rollback");
        return result;
    }
    if(!SRAdmin_DBExec("COMMIT", "ban_commit_check_required")) return 0;
    printf("[ADMIN] BAN_APPLIED actor=%d account=%d target_account=%d days=%d", playerid, GetPlayerAccountID(playerid), account, days);
    return 1;
}

// Internal transaction participant. Call only after the caller has locked and checked both accounts.
// Also used for the donor's automatic ten-day ban on a third warning, available at level 2.
stock SRAdmin_WriteBanLocked(playerid, account, days, ip[], reason[])
{
    new query[768], now = gettime();
    mysql_format(mysql, query, sizeof query,
        "SELECT (SELECT COUNT(*) FROM listban WHERE acc_id=%d AND (unban_time=0 OR unban_time>%d)) + (SELECT COUNT(*) FROM ban_list WHERE user_id=%d AND (ban_time=0 OR ban_time>%d)) AS active_bans",
        account, now, account, now);
    new Cache:result = mysql_query(mysql, query);
    new error = mysql_errno(mysql);
    new existing = error ? 0 : cache_get_field_content_int(0, "active_bans", mysql);
    if(result != Cache:0) cache_delete(result, mysql);
    if(error || existing)
    {
        return existing ? -1 : 0;
    }
    // Donor counts expiry from the beginning of the current day.
    new begin = now - (now % 86400), end = begin + days * 86400;
    new short_reason[33];
    format(short_reason, sizeof short_reason, "%s", reason);
    mysql_format(mysql, query, sizeof query, "DELETE FROM ban_list WHERE user_id=%d", account);
    new ok = SRAdmin_DBExec(query, "ban_remove_expired_legacy");
    if(ok)
    {
        mysql_format(mysql, query, sizeof query, "DELETE FROM listban WHERE acc_id=%d", account);
        ok = SRAdmin_DBExec(query, "ban_remove_expired_primary");
    }
    if(ok)
    {
        mysql_format(mysql, query, sizeof query,
            "INSERT INTO ban_list (user_id,time,ban_time,ip,description,admin) VALUES (%d,%d,%d,'%e','%e','%e')",
            account, begin, end, ip, short_reason, GetPlayerNameEx(playerid));
        ok = SRAdmin_DBExec(query, "ban_insert_legacy");
    }
    if(ok)
    {
        mysql_format(mysql, query, sizeof query,
            "INSERT INTO listban (acc_id,player_name,admin_name,reason,ban_time,unban_time) SELECT id,name,'%e','%e',%d,%d FROM accounts WHERE id=%d",
            GetPlayerNameEx(playerid), reason, begin, end, account);
        new affected;
        ok = SRAdmin_DBChange(query, "ban_insert_primary", affected) && affected == 1;
    }
    return ok;
}

stock SRAdmin_FindAccount(name[], &account, &level, ip[], ip_size)
{
    new query[192];
    mysql_format(mysql, query, sizeof query, "SELECT id,admin,last_ip FROM accounts WHERE name='%e' LIMIT 1", name);
    new Cache:result = mysql_query(mysql, query);
    new error = mysql_errno(mysql);
    new rows = error ? 0 : cache_get_row_count(mysql);
    account = 0;
    level = 0;
    ip[0] = EOS;
    if(rows == 1)
    {
        account = cache_get_field_content_int(0, "id", mysql);
        level = cache_get_field_content_int(0, "admin", mysql);
        cache_get_field_content(0, "last_ip", ip, mysql, ip_size);
    }
    if(result != Cache:0) cache_delete(result, mysql);
    if(error) printf("[ADMIN] ACCOUNT_LOOKUP_FAILED errno=%d", error);
    return !error && rows == 1 && account > 0;
}

stock SRAdmin_UnbanAccount(playerid, account)
{
    if(!SRAdmin_Require(playerid, 3) || account <= 0) return 0;
    if(!SRAdmin_BanTransaction()) return 0;
    if(!SRAdmin_LockBanAccounts(playerid, account, 3))
    {
        SRAdmin_DBExec("ROLLBACK", "unban_rights_rollback");
        return 0;
    }
    new query[160], affected, changed;
    mysql_format(mysql, query, sizeof query, "DELETE FROM listban WHERE acc_id=%d", account);
    new ok = SRAdmin_DBChange(query, "unban_delete_primary", changed);
    if(ok) affected += changed;
    if(ok)
    {
        mysql_format(mysql, query, sizeof query, "DELETE FROM ban_list WHERE user_id=%d", account);
        ok = SRAdmin_DBChange(query, "unban_delete_legacy", changed);
        if(ok) affected += changed;
    }
    if(!ok || affected == 0)
    {
        SRAdmin_DBExec("ROLLBACK", "unban_rollback");
        return ok ? -1 : 0;
    }
    if(!SRAdmin_DBExec("COMMIT", "unban_commit_check_required")) return 0;
    printf("[ADMIN] UNBAN_APPLIED actor=%d account=%d target_account=%d", playerid, GetPlayerAccountID(playerid), account);
    return 1;
}

stock SRAdmin_Target(playerid, targetid, level)
{
    if(!SRAdmin_Require(playerid, level)) return 0;
    if(targetid < 0 || targetid >= MAX_PLAYERS || !IsPlayerConnected(targetid) || !IsPlayerLogged(targetid))
    {
        SendClientMessage(playerid, -1, "Игрок не найден.");
        return 0;
    }
    if((GetPlayerAdminEx(targetid) > 0 && SRAdmin_Level(targetid) == 0) || SRAdmin_Level(targetid) > SRAdmin_Level(playerid))
    {
        SendClientMessage(playerid, -1, "У цели административный уровень выше вашего.");
        return 0;
    }
    return 1;
}

stock SRAdmin_SafeKick(playerid)
{
    // No delayed playerid-only callback: a later occupant of the slot cannot be kicked.
    return Kick(playerid);
}

stock SRAdmin_SaveTimer(playerid, targetid, value, sr_is_jail)
{
    if(!SRAdmin_Target(playerid, targetid, 1)) return 0;
    if(value < 0 || value > 360 * 60) return 0;
    new field[8], query[192];
    if(sr_is_jail) format(field, sizeof field, "jail");
    else format(field, sizeof field, "mute");
    mysql_format(mysql, query, sizeof query,
        "UPDATE accounts SET `%s`=%d WHERE id=%d AND admin=%d LIMIT 1",
        field, value, GetPlayerAccountID(targetid), GetPlayerAdminEx(targetid));
    new affected;
    if(!SRAdmin_DBChange(query, "punishment_save", affected)) return 0;
    if(affected == 1) return 1;
    // MySQL can report zero changed rows when the desired value is already stored.
    mysql_format(mysql, query, sizeof query,
        "SELECT id FROM accounts WHERE id=%d AND admin=%d AND `%s`=%d LIMIT 1",
        GetPlayerAccountID(targetid), GetPlayerAdminEx(targetid), field, value);
    new Cache:result = mysql_query(mysql, query);
    new error = mysql_errno(mysql);
    new rows = error ? 0 : cache_get_row_count(mysql);
    if(result != Cache:0) cache_delete(result, mysql);
    return !error && rows == 1;
}

stock SRAdmin_SaveWarning(playerid, targetid, remove, &warn_count)
{
    if(!SRAdmin_Target(playerid, targetid, 2)) return 0;
    if(!SRAdmin_BanTransaction()) return 0;
    new account = GetPlayerAccountID(targetid);
    if(!SRAdmin_LockBanAccounts(playerid, account, 2))
    {
        SRAdmin_DBExec("ROLLBACK", "warn_rights_rollback");
        return 0;
    }
    new query[256], count, expiry;
    mysql_format(mysql, query, sizeof query, "SELECT warn,warn_time FROM accounts WHERE id=%d", account);
    new Cache:result = mysql_query(mysql, query);
    new error = mysql_errno(mysql);
    new rows = error ? 0 : cache_get_row_count(mysql);
    if(rows == 1)
    {
        count = cache_get_field_content_int(0, "warn", mysql);
        expiry = cache_get_field_content_int(0, "warn_time", mysql);
    }
    if(result != Cache:0) cache_delete(result, mysql);
    if(error || rows != 1 || count < 0)
    {
        SRAdmin_DBExec("ROLLBACK", "warn_read_rollback");
        return 0;
    }
    // Expired warnings must not unexpectedly become an automatic ban.
    if(expiry > 0 && expiry <= gettime()) count = 0;
    if(remove) count = 0;
    else if(count < 3) count++;
    warn_count = count;
    new automatic_ban = !remove && count >= 3;
    if(automatic_ban && SRAdmin_WriteBanLocked(playerid, account, 10, GetPlayerIpEx(targetid), "получено 3 варна") != 1)
    {
        SRAdmin_DBExec("ROLLBACK", "warn_ban_rollback");
        return 0;
    }
    if(automatic_ban) count = 0;
    expiry = count > 0 ? gettime() + 86400 * 10 : 0;
    if(remove)
        mysql_format(mysql, query, sizeof query, "UPDATE accounts SET warn=0,warn_time=0 WHERE id=%d LIMIT 1", account);
    else
        mysql_format(mysql, query, sizeof query, "UPDATE accounts SET warn=%d,warn_time=%d,team=0,org_skin=0,job=0 WHERE id=%d LIMIT 1", count, expiry, account);
    if(!SRAdmin_DBExec(query, "warn_save"))
    {
        SRAdmin_DBExec("ROLLBACK", "warn_save_rollback");
        return 0;
    }
    if(!SRAdmin_DBExec("COMMIT", "warn_commit_check_required")) return 0;
    SetPlayerData(targetid, P_WARN, count);
    SetPlayerData(targetid, P_WARN_TIME, expiry);
    if(!remove)
    {
        // Persisted above together with the warning; reuse the local organisation runtime API.
        InvitePlayer(targetid, 0, 0, false);
        SetPlayerData(targetid, P_OSKIN, 0);
        ResetSkin(targetid);
    }
    printf("[ADMIN] WARN_SAVED actor=%d account=%d target_account=%d count=%d cleared=%d automatic_ban=%d", playerid, GetPlayerAccountID(playerid), account, warn_count, remove, automatic_ban);
    return 1;
}

// ======================================================================
// af
// ======================================================================
#define SRAdmin_AFDialogResponse SRAdmin_AFDialogResponse_Base
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
#undef SRAdmin_AFDialogResponse

// ======================================================================
// af_quick
// ======================================================================
// /af: после выбора типа наказания - одна строка ввода вместо формы с блоками.
//   со сроком (Mute, Jail, Ban):      ник/id срок причина     (Vasya_Pupkin 7 DM)
//   без срока (Warn, Kick, Permban):  ник/id причина          (Vasya_Pupkin DM)
// Подключается СРАЗУ ПОСЛЕ admin_skarf_af.inc (см. блок include в icerussia.pwn): в pwn оригинальная
// SRAdmin_AFDialogResponse из af.inc переименована в SRAdmin_AFDialogResponse_Base, а здесь определена новая,
// которая сама обрабатывает выбор типа и строку ввода, а всё остальное отдаёт в _Base.
// Сам admin_skarf_af.inc менять не нужно.
#if defined _sr_admin_af_quick_included
    #endinput
#endif
#define _sr_admin_af_quick_included

#define SR_AF_STAGE_QUICK       (7)         // строка ввода "ник/id [срок] причина"

stock SRAdmin_AFShowQuick(playerid)
{
    new type = GetPVarInt(playerid, "SRAfType");
    if(type < 0 || type >= SR_AF_TYPES) return SRAdmin_AFShowTypes(playerid);

    new title[64], text[640], author[40];
    SRAdmin_AFAuthor(GetPlayerNameEx(playerid), author, sizeof author);
    format(title, sizeof title, "Форма: %s", g_SRAFType[type][SRAT_Name]);

    if(g_SRAFType[type][SRAT_HasTerm])
    {
        new sample = (g_SRAFType[type][SRAT_TermMax] < 15) ? g_SRAFType[type][SRAT_TermMax] : 15;
        format(text, sizeof text,
            "{FFFFFF}Введите одной строкой:\n{FFCD00}ник/id срок причина\n\n{FFFFFF}Срок: от {FFCD00}%d{FFFFFF} до {FFCD00}%d{FFFFFF} %s\nПример: {FFCD00}Vasya_Pupkin %d DM\n\n{FFFFFF}Причина - до {FFCD00}%d{FFFFFF} символов, в конце добавится {FFCD00}| By %s",
            g_SRAFType[type][SRAT_TermMin], g_SRAFType[type][SRAT_TermMax], g_SRAFType[type][SRAT_Unit],
            sample, SRAdmin_AFReasonLimit(playerid), author);
    }
    else
    {
        format(text, sizeof text,
            "{FFFFFF}Введите одной строкой:\n{FFCD00}ник/id причина\n\n{FFFFFF}Срок не указывается.\nПример: {FFCD00}Vasya_Pupkin DM\n\n{FFFFFF}Причина - до {FFCD00}%d{FFFFFF} символов, в конце добавится {FFCD00}| By %s",
            SRAdmin_AFReasonLimit(playerid), author);
    }
    SetPVarInt(playerid, "SRAfStage", SR_AF_STAGE_QUICK);
    return Dialog(playerid, DIALOG_AF_VIEW, DIALOG_STYLE_INPUT, title, text, "Отправить", "Назад");
}

stock SRAdmin_AFDialogResponse(playerid, dialogid, response, listitem, const inputtext[] = "")
{
    new stage = GetPVarInt(playerid, "SRAfStage");

    // 1) Выбор типа наказания -> сразу строка ввода.
    if(dialogid == DIALOG_AF_LIST && stage == SR_AF_STAGE_TYPES && response)
    {
        DeletePVar(playerid, "SRAfStage");
        if(!SRAdmin_Require(playerid, SR_AF_MIN_LEVEL))
        {
            SRAdmin_AFReset(playerid);
            return 1;
        }
        if(listitem < 0 || listitem >= SR_AF_TYPES) return SRAdmin_AFShowTypes(playerid);
        SRAdmin_AFDraftClear(playerid);
        SetPVarInt(playerid, "SRAfType", listitem);
        return SRAdmin_AFShowQuick(playerid);
    }

    // 2) Ответ на строку ввода.
    if(dialogid == DIALOG_AF_VIEW && stage == SR_AF_STAGE_QUICK)
    {
        DeletePVar(playerid, "SRAfStage");
        if(!SRAdmin_Require(playerid, SR_AF_MIN_LEVEL))
        {
            SRAdmin_AFReset(playerid);
            return 1;
        }
        if(!response) return SRAdmin_AFShowTypes(playerid);

        new type = GetPVarInt(playerid, "SRAfType");
        if(type < 0 || type >= SR_AF_TYPES) return SRAdmin_AFShowTypes(playerid);

        new af_line[192];
        format(af_line, sizeof af_line, "%s", inputtext);
        utf8_to_cp1251(af_line);
        SRAdmin_AFCleanText(af_line);
        SRAdmin_AFTrim(af_line);

        new who[MAX_PLAYER_NAME + 1], term, reason[192];
        if(g_SRAFType[type][SRAT_HasTerm])
        {
            if(sscanf(af_line, "s[25]ds[192]", who, term, reason))
            {
                SendClientMessage(playerid, -1, "Формат: ник/id срок причина");
                return SRAdmin_AFShowQuick(playerid);
            }
        }
        else if(sscanf(af_line, "s[25]s[192]", who, reason))
        {
            SendClientMessage(playerid, -1, "Формат: ник/id причина");
            return SRAdmin_AFShowQuick(playerid);
        }

        // Число до 3 цифр - это ID игрока в сети, иначе ник.
        new length = strlen(who), numeric = (length > 0 && length <= 3);
        for(new i; i < length; i++)
            if(who[i] < '0' || who[i] > '9') numeric = 0;
        if(numeric)
        {
            new targetid = strval(who);
            if(targetid < 0 || targetid >= MAX_PLAYERS || !IsPlayerConnected(targetid) || !IsPlayerLogged(targetid))
            {
                SendClientMessage(playerid, -1, "Игрок с таким ID не найден.");
                return SRAdmin_AFShowQuick(playerid);
            }
            format(who, sizeof who, "%s", GetPlayerNameEx(targetid));
        }

        // Срок, длину причины, права и лимиты проверяет SRAdmin_AFCreate; при ошибке окно откроется снова.
        if(SRAdmin_AFCreate(playerid, type, who, term, reason))
        {
            SRAdmin_AFReset(playerid);
            return 1;
        }
        return SRAdmin_AFShowQuick(playerid);
    }

    // 3) Всё остальное (меню, список форм, просмотр, принять/отклонить) - оригинальный обработчик из af.inc.
    return SRAdmin_AFDialogResponse_Base(playerid, dialogid, response, listitem, inputtext);
}

// ======================================================================
// helper
// ======================================================================
// Система помощников (хелперов): /sethelper, /delhelper, /ask, /hc, /hskin, /hhelp + счётчик ASK: на экране.
// Хелпер = уже существующее поле accounts.helper (P_HELPER >= 1), поэтому купленные "support-права" работают так же.
// Вопросы приходят из окна "Связь -> Связь с помощниками (вопрос)" (диалог 9183), ставятся в очередь и разбираются по /ask.
// Подключать ПОСЛЕ admin_skarf_menu.inc (нужен SRAdmin_WriteLog) и admin_skarf_core.inc (SRAdmin_Require).
#if defined _sr_admin_helper_included
    #endinput
#endif
#define _sr_admin_helper_included

#if !defined DIALOG_HELPER_ASK
    #define DIALOG_HELPER_ASK       (31903)
    #define DIALOG_HELPER_HELP      (31904)
#endif

#define SR_HELPER_MIN_ADMIN         (7)         // /sethelper и /delhelper: админ-уровень 7+
#define SR_HELPER_QUEUE_MAX         (64)        // длина очереди вопросов
#define SR_HELPER_TEXT_MAX          (144)       // как у вопроса в диалоге 9183
#define SR_HELPER_QUESTION_TTL      (900)       // вопрос старше 15 минут считается устаревшим
#define SR_HELPER_SKIN_MALE         (5500121)
#define SR_HELPER_SKIN_FEMALE       (5500122)
#define SR_HELPER_COLOR             (0x99CC00FF)
#define SR_HELPER_ERROR_COLOR       (0xFF5533FF)

new Text:g_HelperAskTD_Admin  = Text:INVALID_TEXT_DRAW;   // под REP: (для администраторов)
new Text:g_HelperAskTD_Helper = Text:INVALID_TEXT_DRAW;   // на месте REP: (для хелперов, у них REP: нет)

new g_HelperQPlayer[SR_HELPER_QUEUE_MAX];
new g_HelperQAccount[SR_HELPER_QUEUE_MAX];
new g_HelperQTime[SR_HELPER_QUEUE_MAX];
new g_HelperQText[SR_HELPER_QUEUE_MAX][SR_HELPER_TEXT_MAX];
new g_HelperQHead;
new g_HelperQCount;

// ---------------------------------------------------------------------------------------------
// Вспомогательное
// ---------------------------------------------------------------------------------------------

// Убираем цветовые коды, '%' и управляющие символы из пользовательского текста.
stock Helper_CleanText(text[])
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

// Может ли игрок работать с вопросами: хелпер, либо администратор, уже вошедший в админ-панель (PIN).
stock Helper_IsSupport(playerid)
{
    if(playerid < 0 || playerid >= MAX_PLAYERS || !IsPlayerConnected(playerid) || !IsPlayerLogged(playerid)) return 0;
    if(GetPlayerAdminEx(playerid) > 0 && GetPlayerData(playerid, P_ADMIN_LOGGED)) return 1;
    return GetPlayerHelperEx(playerid) >= 1;
}

stock Helper_SendToSupport(color, const message[], exceptid = INVALID_PLAYER_ID)
{
    new count;
    foreach(new i : Player)
    {
        if(i == exceptid || !Helper_IsSupport(i)) continue;
        SendClientMessage(i, color, message);
        count++;
    }
    return count;
}

// ---------------------------------------------------------------------------------------------
// Очередь вопросов
// ---------------------------------------------------------------------------------------------

stock Helper_SlotAlive(slot)
{
    new pid = g_HelperQPlayer[slot];
    if(pid < 0 || pid >= MAX_PLAYERS || !IsPlayerConnected(pid)) return 0;
    if(!IsPlayerLogged(pid) || GetPlayerAccountID(pid) != g_HelperQAccount[slot]) return 0;
    if(gettime() - g_HelperQTime[slot] > SR_HELPER_QUESTION_TTL) return 0;
    return 1;
}

// Выкидывает из очереди вышедших игроков и устаревшие вопросы, порядок сохраняется.
stock Helper_QueuePurge()
{
    new kept;
    for(new i; i < g_HelperQCount; i++)
    {
        new src = (g_HelperQHead + i) % SR_HELPER_QUEUE_MAX;
        if(!Helper_SlotAlive(src)) continue;
        new dst = (g_HelperQHead + kept) % SR_HELPER_QUEUE_MAX;
        if(dst != src)
        {
            g_HelperQPlayer[dst] = g_HelperQPlayer[src];
            g_HelperQAccount[dst] = g_HelperQAccount[src];
            g_HelperQTime[dst] = g_HelperQTime[src];
            format(g_HelperQText[dst], SR_HELPER_TEXT_MAX, "%s", g_HelperQText[src]);
        }
        kept++;
    }
    g_HelperQCount = kept;
    return 1;
}

// Сколько вопросов ждёт ответа (в очереди + те, которые хелпер уже открыл, но ещё не ответил).
stock Helper_CountPending()
{
    new count;
    for(new i; i < g_HelperQCount; i++)
        if(Helper_SlotAlive((g_HelperQHead + i) % SR_HELPER_QUEUE_MAX)) count++;

    foreach(new pid : Player)
    {
        if(!GetPVarInt(pid, "HELPER_ASK_ACTIVE")) continue;
        new target = GetPVarInt(pid, "HELPER_ASK_TARGET");
        if(target < 0 || target >= MAX_PLAYERS || !IsPlayerConnected(target)) continue;
        if(GetPlayerAccountID(target) != GetPVarInt(pid, "HELPER_ASK_ACCOUNT")) continue;
        count++;
    }
    return count;
}

stock Helper_UpdateCounter()
{
    new text[16];
    format(text, sizeof text, "ASK:_%d", Helper_CountPending());
    if(g_HelperAskTD_Admin != Text:INVALID_TEXT_DRAW) TextDrawSetString(g_HelperAskTD_Admin, text);
    if(g_HelperAskTD_Helper != Text:INVALID_TEXT_DRAW) TextDrawSetString(g_HelperAskTD_Helper, text);
    return 1;
}

stock Helper_EnqueueQuestion(playerid, const text[])
{
    if(g_HelperQCount >= SR_HELPER_QUEUE_MAX) Helper_QueuePurge();
    if(g_HelperQCount >= SR_HELPER_QUEUE_MAX) return 0;

    new slot = (g_HelperQHead + g_HelperQCount) % SR_HELPER_QUEUE_MAX;
    g_HelperQPlayer[slot] = playerid;
    g_HelperQAccount[slot] = GetPlayerAccountID(playerid);
    g_HelperQTime[slot] = gettime();
    format(g_HelperQText[slot], SR_HELPER_TEXT_MAX, "%s", text);
    g_HelperQCount++;
    Helper_UpdateCounter();
    return 1;
}

// Возврат вопроса в НАЧАЛО очереди (хелпер нажал "Отмена" или вышел из игры).
stock Helper_PushFront(playerid, const text[])
{
    if(g_HelperQCount >= SR_HELPER_QUEUE_MAX) Helper_QueuePurge();
    if(g_HelperQCount >= SR_HELPER_QUEUE_MAX) return 0;

    g_HelperQHead = (g_HelperQHead + SR_HELPER_QUEUE_MAX - 1) % SR_HELPER_QUEUE_MAX;
    new slot = g_HelperQHead;
    g_HelperQPlayer[slot] = playerid;
    g_HelperQAccount[slot] = GetPlayerAccountID(playerid);
    g_HelperQTime[slot] = gettime();
    format(g_HelperQText[slot], SR_HELPER_TEXT_MAX, "%s", text);
    g_HelperQCount++;
    return 1;
}

// Берёт первый живой вопрос; мёртвые пропускает.
stock Helper_Dequeue(&targetid, text[], len)
{
    while(g_HelperQCount > 0)
    {
        new slot = g_HelperQHead;
        g_HelperQHead = (g_HelperQHead + 1) % SR_HELPER_QUEUE_MAX;
        g_HelperQCount--;
        if(!Helper_SlotAlive(slot)) continue;
        targetid = g_HelperQPlayer[slot];
        format(text, len, "%s", g_HelperQText[slot]);
        return 1;
    }
    return 0;
}

// ---------------------------------------------------------------------------------------------
// Открытый у хелпера вопрос
// ---------------------------------------------------------------------------------------------

stock Helper_ClearActive(playerid)
{
    DeletePVar(playerid, "HELPER_ASK_ACTIVE");
    DeletePVar(playerid, "HELPER_ASK_TARGET");
    DeletePVar(playerid, "HELPER_ASK_ACCOUNT");
    DeletePVar(playerid, "HELPER_ASK_TEXT");
    return 1;
}

stock Helper_ReturnActive(playerid)
{
    if(!GetPVarInt(playerid, "HELPER_ASK_ACTIVE")) return 0;

    new target = GetPVarInt(playerid, "HELPER_ASK_TARGET");
    new account = GetPVarInt(playerid, "HELPER_ASK_ACCOUNT");
    new text[SR_HELPER_TEXT_MAX];
    GetPVarString(playerid, "HELPER_ASK_TEXT", text, sizeof text);
    Helper_ClearActive(playerid);

    if(target >= 0 && target < MAX_PLAYERS && IsPlayerConnected(target)
        && GetPlayerAccountID(target) == account && strlen(text))
    {
        Helper_PushFront(target, text);
    }
    Helper_UpdateCounter();
    return 1;
}

stock Helper_ShowActive(playerid)
{
    new target = GetPVarInt(playerid, "HELPER_ASK_TARGET");
    new text[SR_HELPER_TEXT_MAX], info[512];
    GetPVarString(playerid, "HELPER_ASK_TEXT", text, sizeof text);

    format(info, sizeof info,
        "{FFFFFF}Игрок: {FFCD00}%s[%d]\n\n{FFFFFF}Вопрос:\n{99CC00}%s\n\n{FFFFFF}Введите ответ в поле ниже:",
        GetPlayerNameEx(target), target, text);
    return Dialog(playerid, DIALOG_HELPER_ASK, DIALOG_STYLE_INPUT,
        ""SERVER_COLOR SERVER_NAME" {FFFFFF}| Вопрос игрока", info, "Ответить", "Отмена");
}

stock Helper_DeliverAnswer(targetid, supportid, const answer[])
{
    if(!IsPlayerConnected(targetid)) return 0;

    new header[160];
    if(GetPlayerAdminEx(supportid) > 0 && GetPlayerData(supportid, P_ADMIN_LOGGED))
        format(header, sizeof header, "{704214}[ASK] {FFFFFF}Администратор %s[%d] ответил на ваш вопрос:", GetPlayerNameEx(supportid), supportid);
    else
        format(header, sizeof header, "{704214}[ASK] {FFFFFF}Хелпер %s[%d] ответил на ваш вопрос:", GetPlayerNameEx(supportid), supportid);
    SendClientMessage(targetid, 0xFFFFFFFF, header);

    // Ограничение длины строки чата: режем по пробелам кусками до ~90 символов.
    new length = strlen(answer), offset;
    while(offset < length)
    {
        new end = offset + 90;
        if(end >= length) end = length;
        else
        {
            new cut = end;
            while(cut > offset + 50 && answer[cut] != ' ') cut--;
            if(answer[cut] == ' ') end = cut;
        }
        new chunk[100];
        strmid(chunk, answer, offset, end, sizeof chunk);
        SendClientMessage(targetid, 0xFFAE42FF, chunk);
        offset = end;
        while(offset < length && answer[offset] == ' ') offset++;
    }
    return 1;
}

// Строка для хелперов и админов: [H] Ник[id] ответил Ник[id]: текст (тег коричневый, ответ оранжевый).
// Строка чата SA-MP ограничена ~144 символами, поэтому длинный ответ переносится на следующие строки (оранжевые).
stock Helper_SendAnswerToSupport(supportid, targetid, const answer[])
{
    new prefix[160];
    format(prefix, sizeof prefix, "{704214}[H] %s[%d] ответил %s[%d]: {FFAE42}", GetPlayerNameEx(supportid), supportid, GetPlayerNameEx(targetid), targetid);

    new length = strlen(answer), offset, first = 1;
    while(offset < length)
    {
        new limit = first ? (138 - strlen(prefix)) : 100;
        if(limit < 30) limit = 30;
        new end = offset + limit;
        if(end >= length) end = length;
        else
        {
            new cut = end;
            while(cut > offset + 15 && answer[cut] != ' ') cut--;
            if(answer[cut] == ' ') end = cut;
        }
        new chunk[128], message[320];
        strmid(chunk, answer, offset, end, sizeof chunk);
        if(first) format(message, sizeof message, "%s%s", prefix, chunk);
        else format(message, sizeof message, "{FFAE42}%s", chunk);
        Helper_SendToSupport(0xFFFFFFFF, message);
        first = 0;
        offset = end;
        while(offset < length && answer[offset] == ' ') offset++;
    }
    return 1;
}

stock Helper_AskResponse(playerid, response, const inputtext[])
{
    if(!GetPVarInt(playerid, "HELPER_ASK_ACTIVE")) return 1;

    // Права могли пропасть, пока окно было открыто (/delhelper, смена уровня): вопрос не теряем.
    if(!Helper_IsSupport(playerid))
    {
        Helper_ReturnActive(playerid);
        return 1;
    }

    if(!response)
    {
        Helper_ReturnActive(playerid);
        SendClientMessage(playerid, 0xCECECEFF, "{FFFF00}| {FFFFFF}Вопрос возвращён в очередь.");
        return 1;
    }

    new answer[256];
    format(answer, sizeof answer, "%s", inputtext);
    utf8_to_cp1251(answer);
    Helper_CleanText(answer);
    if(strlen(answer) < 2)
    {
        SendClientMessage(playerid, 0xCECECEFF, "Введите текст ответа.");
        return Helper_ShowActive(playerid);
    }
    if(strlen(answer) >= SR_HELPER_TEXT_MAX)
    {
        SendClientMessage(playerid, 0xCECECEFF, "Ответ слишком длинный (максимум 143 символа).");
        return Helper_ShowActive(playerid);
    }

    new targetid = GetPVarInt(playerid, "HELPER_ASK_TARGET");
    new account = GetPVarInt(playerid, "HELPER_ASK_ACCOUNT");
    new question[SR_HELPER_TEXT_MAX];
    GetPVarString(playerid, "HELPER_ASK_TEXT", question, sizeof question);
    Helper_ClearActive(playerid);

    if(targetid < 0 || targetid >= MAX_PLAYERS || !IsPlayerConnected(targetid) || GetPlayerAccountID(targetid) != account)
    {
        Helper_UpdateCounter();
        return SendClientMessage(playerid, 0xCECECEFF, "{FFFF00}| {FFFFFF}Игрок, задавший вопрос, уже вышел из игры.");
    }

    Helper_DeliverAnswer(targetid, playerid, answer);
    SendClientMessage(playerid, SR_HELPER_COLOR, "{FFFF00}| {FFFFFF}Ответ отправлен игроку.");

    // Строка как у репорта: [H] Ник[id] ответил Ник[id]: текст - видят все хелперы и админы в сети (включая отвечающего).
    Helper_SendAnswerToSupport(playerid, targetid, answer);

    new log_text[400];
    format(log_text, sizeof log_text, "Ask answer: target=%s account=%d question=%s answer=%s", GetPlayerNameEx(targetid), account, question, answer);
    SRAdmin_WriteLog(playerid, LOG_TYPE_ADMIN_ACTION, log_text);

    Helper_UpdateCounter();
    return 1;
}

stock Helper_DialogResponse(playerid, dialogid, response, const inputtext[])
{
    if(dialogid == DIALOG_HELPER_ASK) return Helper_AskResponse(playerid, response, inputtext);
    return 1; // DIALOG_HELPER_HELP: просто информационное окно
}

// ---------------------------------------------------------------------------------------------
// Надпись ASK: на экране
// ---------------------------------------------------------------------------------------------

stock Helper_StyleTextDraw(Text:td)
{
    TextDrawLetterSize(td, 0.400000, 1.600000);
    TextDrawAlignment(td, 1);
    TextDrawColor(td, -1);
    TextDrawSetShadow(td, 0);
    TextDrawSetOutline(td, 1);
    TextDrawBackgroundColor(td, 255);
    TextDrawFont(td, 1);
    TextDrawSetProportional(td, 1);
    return 1;
}

// Вызывать один раз из OnGameModeInit (сразу после создания report_edit_TD).
stock Helper_CreateTextDraws()
{
    if(g_HelperAskTD_Admin != Text:INVALID_TEXT_DRAW) TextDrawDestroy(g_HelperAskTD_Admin);
    if(g_HelperAskTD_Helper != Text:INVALID_TEXT_DRAW) TextDrawDestroy(g_HelperAskTD_Helper);

    // REP: стоит в (7.5; 314.44), ASK: идёт строкой ниже.
    g_HelperAskTD_Admin = TextDrawCreate(7.499990, 331.444396, "ASK:_0");
    Helper_StyleTextDraw(g_HelperAskTD_Admin);
    g_HelperAskTD_Helper = TextDrawCreate(7.499990, 314.444396, "ASK:_0");
    Helper_StyleTextDraw(g_HelperAskTD_Helper);
    return 1;
}

// Раз в секунду из OnPlayerTimer: показываем/прячем надпись по правам игрока.
stock Helper_OnPlayerTimer(playerid)
{
    if(g_HelperAskTD_Admin == Text:INVALID_TEXT_DRAW || g_HelperAskTD_Helper == Text:INVALID_TEXT_DRAW) return 0;

    new logged = IsPlayerLogged(playerid) ? 1 : 0;
    new admin_view = logged && GetPlayerAdminEx(playerid) && GetPlayerData(playerid, P_ADMIN_LOGGED);
    new helper_view = logged && !admin_view && GetPlayerHelperEx(playerid) >= 1;

    if(admin_view) TextDrawShowForPlayer(playerid, g_HelperAskTD_Admin);
    else TextDrawHideForPlayer(playerid, g_HelperAskTD_Admin);

    if(helper_view) TextDrawShowForPlayer(playerid, g_HelperAskTD_Helper);
    else TextDrawHideForPlayer(playerid, g_HelperAskTD_Helper);
    return 1;
}

// Из OnPlayerDisconnect: открытый вопрос возвращается в очередь.
stock Helper_OnDisconnect(playerid)
{
    Helper_ReturnActive(playerid);
    DeletePVar(playerid, "HELPER_SKIN");
    Helper_UpdateCounter();
    return 1;
}

// ---------------------------------------------------------------------------------------------
// Команды администрации: /sethelper, /delhelper
// ---------------------------------------------------------------------------------------------

CMD:sethelper(playerid, params[])
{
    if(!SRAdmin_Require(playerid, SR_HELPER_MIN_ADMIN)) return 1;

    new targetid;
    if(sscanf(params, "d", targetid))
        return SendClientMessage(playerid, 0xCECECEFF, "Используйте: /sethelper [id игрока]");
    if(targetid < 0 || targetid >= MAX_PLAYERS || !IsPlayerConnected(targetid) || !IsPlayerLogged(targetid))
        return SendClientMessage(playerid, -1, "Игрок не найден.");
    if(GetPlayerAdminEx(targetid) > 0)
        return SendClientMessage(playerid, 0xCECECEFF, "У игрока уже есть админ-права, хелперка ему не нужна.");
    if(GetPlayerHelperEx(targetid) >= 1)
        return SendClientMessage(playerid, 0xCECECEFF, "У игрока уже есть права хелпера.");

    SetPlayerData(targetid, P_HELPER, 1);
    UpdatePlayerDatabaseInt(targetid, "helper", 1);

    new message[192];
    format(message, sizeof message, "{FFFF00}| {FFFFFF}Администратор %s[%d] выдал вам права хелпера. Список команд: {FFCD00}/hhelp", GetPlayerNameEx(playerid), playerid);
    SendClientMessage(targetid, SR_HELPER_COLOR, message);

    format(message, sizeof message, "[A] %s[%d] выдал хелперку игроку %s[%d]", GetPlayerNameEx(playerid), playerid, GetPlayerNameEx(targetid), targetid);
    SendMessageToAdmins(message, 0xFF5533FF);
    SRAdmin_WriteLog(playerid, LOG_TYPE_ADMIN_ACTION, message);
    printf("[ADMIN] HELPER_SET actor=%d actor_account=%d target_account=%d", playerid, GetPlayerAccountID(playerid), GetPlayerAccountID(targetid));
    Helper_UpdateCounter();
    return 1;
}

CMD:delhelper(playerid, params[])
{
    if(!SRAdmin_Require(playerid, SR_HELPER_MIN_ADMIN)) return 1;

    new targetid;
    if(sscanf(params, "d", targetid))
        return SendClientMessage(playerid, 0xCECECEFF, "Используйте: /delhelper [id игрока]");
    if(targetid < 0 || targetid >= MAX_PLAYERS || !IsPlayerConnected(targetid) || !IsPlayerLogged(targetid))
        return SendClientMessage(playerid, -1, "Игрок не найден.");
    if(GetPlayerHelperEx(targetid) < 1)
        return SendClientMessage(playerid, 0xCECECEFF, "У игрока нет прав хелпера.");

    // Открытый вопрос возвращаем в очередь, скин хелпера снимаем.
    Helper_ReturnActive(targetid);
    if(GetPVarInt(targetid, "HELPER_SKIN"))
    {
        DeletePVar(targetid, "HELPER_SKIN");
        ResetSkin(targetid);
    }
    SetPlayerData(targetid, P_HELPER, 0);
    UpdatePlayerDatabaseInt(targetid, "helper", 0);

    new message[192];
    format(message, sizeof message, "{FFFF00}| {FFFFFF}Администратор %s[%d] снял с вас права хелпера.", GetPlayerNameEx(playerid), playerid);
    SendClientMessage(targetid, SR_HELPER_ERROR_COLOR, message);

    format(message, sizeof message, "[A] %s[%d] снял хелперку у игрока %s[%d]", GetPlayerNameEx(playerid), playerid, GetPlayerNameEx(targetid), targetid);
    SendMessageToAdmins(message, 0xFF5533FF);
    SRAdmin_WriteLog(playerid, LOG_TYPE_ADMIN_ACTION, message);
    printf("[ADMIN] HELPER_DEL actor=%d actor_account=%d target_account=%d", playerid, GetPlayerAccountID(playerid), GetPlayerAccountID(targetid));
    Helper_UpdateCounter();
    return 1;
}

// ---------------------------------------------------------------------------------------------
// Команды хелпера: /ask, /hc, /hskin, /hhelp
// ---------------------------------------------------------------------------------------------

CMD:ask(playerid, params[])
{
    #pragma unused params
    if(!Helper_IsSupport(playerid))
    {
        // Администратору без PIN-входа покажем окно входа, остальным - отказ.
        if(IsPlayerLogged(playerid) && GetPlayerAdminEx(playerid) > 0) SRAdmin_Require(playerid, 1);
        else SendClientMessage(playerid, SR_HELPER_ERROR_COLOR, "У вас нет доступа к этой команде.");
        return 1;
    }

    // Вопрос уже открыт - просто показываем окно ещё раз.
    if(GetPVarInt(playerid, "HELPER_ASK_ACTIVE"))
    {
        new active = GetPVarInt(playerid, "HELPER_ASK_TARGET");
        if(active >= 0 && active < MAX_PLAYERS && IsPlayerConnected(active)
            && GetPlayerAccountID(active) == GetPVarInt(playerid, "HELPER_ASK_ACCOUNT"))
            return Helper_ShowActive(playerid);
        Helper_ClearActive(playerid); // игрок вышел, пока вопрос висел
    }

    new targetid = INVALID_PLAYER_ID, question[SR_HELPER_TEXT_MAX];
    if(!Helper_Dequeue(targetid, question, sizeof question))
    {
        Helper_UpdateCounter();
        return SendClientMessage(playerid, 0xCECECEFF, "{FFFF00}| {FFFFFF}Новых вопросов от игроков пока нет.");
    }

    SetPVarInt(playerid, "HELPER_ASK_ACTIVE", 1);
    SetPVarInt(playerid, "HELPER_ASK_TARGET", targetid);
    SetPVarInt(playerid, "HELPER_ASK_ACCOUNT", GetPlayerAccountID(targetid));
    SetPVarString(playerid, "HELPER_ASK_TEXT", question);
    Helper_UpdateCounter();
    return Helper_ShowActive(playerid);
}

CMD:hc(playerid, params[])
{
    if(!Helper_IsSupport(playerid))
        return SendClientMessage(playerid, SR_HELPER_ERROR_COLOR, "У вас нет доступа к этой команде.");
    if(IsPlayerMuted(playerid))
        return SendClientMessage(playerid, 0xFF6600FF, "У вас бан чата!");
    if(!strlen(params))
        return SendClientMessage(playerid, 0xCECECEFF, "Используйте: /hc [текст]");
    if(strlen(params) > 80)
        return SendClientMessage(playerid, 0xCECECEFF, "Максимальная длина сообщения в /hc: 80 символов.");

    new text[96], message[192];
    format(text, sizeof text, "%s", params);
    Helper_CleanText(text);

    // Вид строки: *Хелпер (!) Ник[id]: текст (тег зелёный, ник и текст белые)
    if(GetPlayerAdminEx(playerid) > 0 && GetPlayerData(playerid, P_ADMIN_LOGGED))
        format(message, sizeof message, "{99CC00}*Админ (!) {FFFFFF}%s[%d]: %s", GetPlayerNameEx(playerid), playerid, text);
    else
        format(message, sizeof message, "{99CC00}*Хелпер (!) {FFFFFF}%s[%d]: %s", GetPlayerNameEx(playerid), playerid, text);
    Helper_SendToSupport(0xFFFFFFFF, message);
    return 1;
}

CMD:hskin(playerid, params[])
{
    #pragma unused params
    if(!IsPlayerLogged(playerid) || GetPlayerHelperEx(playerid) < 1)
        return SendClientMessage(playerid, SR_HELPER_ERROR_COLOR, "У вас нет доступа к этой команде.");

    if(GetPVarInt(playerid, "HELPER_SKIN"))
    {
        DeletePVar(playerid, "HELPER_SKIN");
        ResetSkin(playerid);
        return SendClientMessage(playerid, SR_HELPER_COLOR, "{FFFF00}| {FFFFFF}Вы сняли скин хелпера.");
    }

    // P_SEX: 0 - мужской, 1 - женский
    SetPlayerSkin(playerid, GetPlayerSex(playerid) ? SR_HELPER_SKIN_FEMALE : SR_HELPER_SKIN_MALE);
    SetPVarInt(playerid, "HELPER_SKIN", 1);
    return SendClientMessage(playerid, SR_HELPER_COLOR, "{FFFF00}| {FFFFFF}Вы надели скин хелпера. Чтобы снять - введите /hskin ещё раз.");
}

CMD:hhelp(playerid, params[])
{
    #pragma unused params
    if(!Helper_IsSupport(playerid))
        return SendClientMessage(playerid, SR_HELPER_ERROR_COLOR, "У вас нет доступа к этой команде.");

    return Dialog(playerid, DIALOG_HELPER_HELP, DIALOG_STYLE_MSGBOX,
        ""SERVER_COLOR SERVER_NAME" {FFFFFF}| Команды хелпера",
        "{FFCD00}/ask {FFFFFF}- взять вопрос игрока из очереди и ответить на него\n\
{FFCD00}/hc [текст] {FFFFFF}- чат хелперов\n\
{FFCD00}/hskin {FFFFFF}- надеть / снять скин хелпера\n\
{FFCD00}/hhelp {FFFFFF}- список команд хелпера\n\n\
{FFFFFF}Счётчик {99CC00}ASK: {FFFFFF}в левой части экрана показывает, сколько вопросов ждёт ответа.",
        "Закрыть", "");
}

// ======================================================================
// setadmin
// ======================================================================
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

// Цвет сообщения выданному администратору (синий).
#define SR_SETADMIN_COLOR_TARGET    (0x3399FFFF)
// Размер префикса админа (P_PREFIX в icerussia.pwn и колонка accounts.prefix): 63 символа + терминатор.
#define SR_PREFIX_SIZE              (64)
// Цвет строк [A-LOGIN] (зелёный). Ник, [id] и должность выводятся тёмно-зелёным (SR_ALOGIN_C_DARK из core), остальной текст - основным.
#define SR_ALOGIN_COLOR             (0x33CC33FF)

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

// Расширить accounts.prefix до VARCHAR(SR_PREFIX_SIZE), сохранив кодировку/NULL/значение по умолчанию. Вызывается из CREATE_ACCOUNTS_TABLE.
stock SRSet_EnsurePrefixColumn()
{
    new Cache:cache = mysql_query(mysql, "SHOW FULL COLUMNS FROM `accounts` LIKE 'prefix'", true);
    if(!cache_num_rows())
    {
        cache_delete(cache);
        return 0;
    }
    new type[32], collation[48], nullable[8], defval[96];
    cache_get_field_content(0, "Type", type, mysql, sizeof type);
    cache_get_field_content(0, "Collation", collation, mysql, sizeof collation);
    cache_get_field_content(0, "Null", nullable, mysql, sizeof nullable);
    cache_get_field_content(0, "Default", defval, mysql, sizeof defval);
    cache_delete(cache);

    // Type имеет вид "varchar(16)"; уже достаточно широкая колонка (или не varchar) не трогается.
    new open = strfind(type, "(");
    if(strfind(type, "varchar") != 0 || open < 0) return 0;
    new width[8];
    strmid(width, type, open + 1, strlen(type), sizeof width);
    if(strval(width) >= SR_PREFIX_SIZE) return 1;

    new query[320], extra[160];
    if(strlen(collation) > 0 && strcmp(collation, "NULL", true)) format(extra, sizeof extra, " COLLATE %s", collation);
    if(!strcmp(nullable, "YES", true)) strcat(extra, " NULL DEFAULT NULL", sizeof extra);
    else
    {
        strcat(extra, " NOT NULL", sizeof extra);
        if(strcmp(defval, "NULL", true))
        {
            new escaped[128];
            mysql_format(mysql, escaped, sizeof escaped, " DEFAULT '%e'", defval);
            strcat(extra, escaped, sizeof extra);
        }
    }
    format(query, sizeof query, "ALTER TABLE `accounts` MODIFY `prefix` VARCHAR(%d)%s", SR_PREFIX_SIZE, extra);
    mysql_query(mysql, query, false);
    if(mysql_errno(mysql)) printf("[ADMIN] PREFIX_COLUMN_ALTER_FAILED errno=%d query=%s", mysql_errno(mysql), query);
    else printf("[ADMIN] accounts.prefix widened to VARCHAR(%d)", SR_PREFIX_SIZE);
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

    new query[384], rank_name[SR_ADMIN_POS_NAME_LEN], prefix[SR_PREFIX_SIZE], account = GetPlayerAccountID(targetid);
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
    // Префикс - вторым запросом: даже если он не сохранится, уровень и должность уже выданы.
    mysql_format(mysql, query, sizeof query, "UPDATE accounts SET prefix='%e' WHERE id=%d LIMIT 1", prefix, account);
    mysql_query(mysql, query, false);
    if(mysql_errno(mysql)) printf("[ADMIN] PREFIX_SAVE_FAILED target_account=%d errno=%d", account, mysql_errno(mysql));

    // End observation while the old permission is still available to its cleanup function.
    if(GetPlayerSpectateData(targetid, S_PLAYER) != -1) StopSpectate(targetid);
    SetPlayerData(targetid, P_ADMIN, level);
    g_SRAdminPosOf[targetid] = pos;
    SetPlayerData(targetid, P_ADMIN_LOGGED, 0);
    SetPlayerColorInit(targetid);
    format(GetPlayerData(targetid, P_PREFIX), SR_PREFIX_SIZE, "%s", prefix);
    SRAdmin_ClearTarget(targetid);
    DeletePVar(targetid, "SRAdmPinStep");

    // Выданному - синее сообщение, действующим администраторам - уведомление о назначении.
    new message[192];
    format(message, sizeof message, "%s[%d] назначил Вас администратором (%s).", GetPlayerNameEx(playerid), playerid, rank_name);
    SendClientMessage(targetid, SR_SETADMIN_COLOR_TARGET, message);
    format(message, sizeof message, "Администратор %s[%d] назначил %s[%d] на пост Администратора (%s)",
        GetPlayerNameEx(playerid), playerid, GetPlayerNameEx(targetid), targetid, rank_name);
    SendMessageToAdmins(message, 0xFF5533FF);
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
        case 8:
        {
            strcat(dest, "/setleader, /setadmin, /sethelper, /aban (заблокировать доступ к админ-панели другому администратору от слива админки; не работает для тех, кто выше по должности)\n", size);
            strcat(dest, "/tradelog [ник игрока] - лог трейдов игрока\n", size);
            strcat(dest, "/getlog [ник игрока] - логи игрока (чат, семья, банк, организация, админ. действия)", size);
        }
        case 9:
        {
            strcat(dest, "/checkinv, /changeproplog, /famlog, /smslog, /chatlog, /banklog, /bylog\n", size);
            strcat(dest, "/tdonate [кол-во] - выдача доната, /tcash [кол-во] - выдача ИВ, /bplevel [кол-во уровней] - выдать уровень БП, /scorebp - выдача БП икспа (работают при включённом Вайт-листе)", size);
        }
        case 10:
        {
            strcat(dest, "/checkinv, /changeproplog, /famlog, /smslog, /chatlog, /banklog, /bylog\n", size);
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

// ---------------------------------------------------------------------------------------------
// Ввод админ-пароля (PIN): замена SRAdmin_PinResponse из admin_skarf_core.inc.
// В icerussia.pwn оригинал переименован макросом (SRAdmin_PinResponse -> SRAdmin_PinResponse_Legacy) на время подключения core.
// Отличие от оригинала только в сообщениях после успешного входа:
//   - вошедшему:        [A-LOGIN]: Вы авторизовались как: "Префикс".
//   - всем админам в сети, включая вошедшего: [A-LOGIN]: Nick[id] авторизовался как "Префикс".
//     (вошедший видит обе строки, остальные - только вторую)
// (Префикс - тот, что поставил /setadmin: название должности.)
// Никаких других строк (IP и т.п.) не отправляется.
// ---------------------------------------------------------------------------------------------
stock SRAdmin_PinResponse(playerid, dialogid, response, const inputtext[])
{
    if(!IsPlayerLogged(playerid) || !SRAdmin_Level(playerid)) return 1;
    if(GetPlayerData(playerid, P_ADMIN_LOGGED) || GetPVarInt(playerid, "SRAdmPinBlocked")) return 1;
    new expected = (dialogid == DIALOG_ADMIN_REG) ? 1 : 2;
    if(GetPVarInt(playerid, "SRAdmPinStep") != expected) return 1;
    DeletePVar(playerid, "SRAdmPinStep");
    if(!response) return 1;
    new length = strlen(inputtext), numeric = length == 4;
    for(new i; i < length; i++)
        if(inputtext[i] < '0' || inputtext[i] > '9') numeric = 0;

    if(expected == 1)
    {
        if(GetPlayerData(playerid, P_APASS) != 0) return 1;
        if(!numeric || length != 4 || strval(inputtext) < 1000)
        {
            SendClientMessage(playerid, 0xFF5533FF, "Административный пароль должен состоять из 4 цифр: от 1000 до 9999.");
            return SRAdmin_ShowLogin(playerid);
        }
        new query[192];
        mysql_format(mysql, query, sizeof query,
            "UPDATE accounts SET AdminPass=%d WHERE id=%d AND AdminPass=0 AND admin=%d LIMIT 1",
            strval(inputtext), GetPlayerAccountID(playerid), GetPlayerAdminEx(playerid));
        new Cache:result = mysql_query(mysql, query);
        new error = mysql_errno(mysql);
        new affected = error ? 0 : cache_affected_rows(mysql);
        if(result != Cache:0) cache_delete(result, mysql);
        if(error || affected != 1)
        {
            printf("[ADMIN] PIN_SAVE_FAILED player=%d account=%d errno=%d affected=%d", playerid, GetPlayerAccountID(playerid), error, affected);
            SendClientMessage(playerid, 0xFF5533FF, "Не удалось сохранить административный пароль. Повторите вход.");
            return 1;
        }
        SetPlayerData(playerid, P_APASS, strval(inputtext));
        return SRAdmin_ShowLogin(playerid);
    }

    if(!numeric || GetPlayerData(playerid, P_APASS) == 0 || strval(inputtext) != GetPlayerData(playerid, P_APASS))
    {
        new attempts = GetPVarInt(playerid, "SRAdmPinAttempts") + 1;
        SetPVarInt(playerid, "SRAdmPinAttempts", attempts);
        printf("[ADMIN] LOGIN_FAILED player=%d account=%d attempt=%d", playerid, GetPlayerAccountID(playerid), attempts);
        if(attempts >= 3)
        {
            SetPVarInt(playerid, "SRAdmPinBlocked", 1);
            return FixKick(playerid);
        }
        SendClientMessage(playerid, 0xFF5533FF, "Неверный административный пароль.");
        return SRAdmin_ShowLogin(playerid);
    }

    DeletePVar(playerid, "SRAdmPinAttempts");
    SetPlayerData(playerid, P_ADMIN_LOGGED, 1);
    RefreshAdminBadge(playerid);

    // В строках входа показывается префикс, выставленный в /setadmin (или изменённый потом через /prefix); если пуст - название должности.
    new rank[SR_PREFIX_SIZE + 8], message[192];
    format(rank, sizeof rank, "%s", GetPlayerData(playerid, P_PREFIX));
    if(!strlen(rank)) SRSet_RankNameFor(playerid, rank, sizeof rank);
    format(message, sizeof message, "[A-LOGIN]: Вы авторизовались как: "SR_ALOGIN_C_DARK"\"%s\".", rank);
    SendClientMessage(playerid, SR_ALOGIN_COLOR, message);
    format(message, sizeof message, "[A-LOGIN]: "SR_ALOGIN_C_DARK"%s[%d] "SR_ALOGIN_C_MAIN"авторизовался как "SR_ALOGIN_C_DARK"\"%s\".", GetPlayerNameEx(playerid), playerid, rank);
    foreach(new i : Player)
    {
        if(!IsPlayerLogged(i) || GetPlayerAdminEx(i) <= 0 || !GetPlayerData(i, P_ADMIN_LOGGED)) continue;
        SendClientMessage(i, SR_ALOGIN_COLOR, message);
    }
    printf("[ADMIN] LOGIN_SUCCESS player=%d account=%d level=%d pos=%d", playerid, GetPlayerAccountID(playerid), SRAdmin_Level(playerid), g_SRAdminPosOf[playerid]);
    return 1;
}
