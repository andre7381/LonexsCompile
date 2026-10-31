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
