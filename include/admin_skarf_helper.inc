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
        format(header, sizeof header, "{99CC00}[ASK] {FFFFFF}Администратор %s[%d] ответил на ваш вопрос:", GetPlayerNameEx(supportid), supportid);
    else
        format(header, sizeof header, "{99CC00}[ASK] {FFFFFF}Помощник %s[%d] ответил на ваш вопрос:", GetPlayerNameEx(supportid), supportid);
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

    new message[192];
    format(message, sizeof message, "{99CC00}[Q] %s[%d] ответил на вопрос игрока %s[%d].", GetPlayerNameEx(playerid), playerid, GetPlayerNameEx(targetid), targetid);
    Helper_SendToSupport(0xFFFFFFFF, message, playerid);

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

    if(GetPlayerAdminEx(playerid) > 0 && GetPlayerData(playerid, P_ADMIN_LOGGED))
        format(message, sizeof message, "[HC] Администратор %s[%d]: %s", GetPlayerNameEx(playerid), playerid, text);
    else
        format(message, sizeof message, "[HC] Хелпер %s[%d]: %s", GetPlayerNameEx(playerid), playerid, text);
    Helper_SendToSupport(SR_HELPER_COLOR, message);
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
