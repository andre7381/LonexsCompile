/*
    ! file encoding: Windows-1251


   ------------------------------------------

    Ñàìûé áûñòğûé INI ğèäåğ/ğàéòåğ ( mxINI )

   ------------------


    ×ÒÎ İÒÎ
        - İòî èíêëóä-ôàéë äëÿ PAWN ñêğèïòîâ ìóëüòèïëååğà GTA San Andreas (SA-MP)

    ÊÀÊ ÈÑÏÎËÜÇÎÂÀÒÜ
        - Ïîìåñòèòü ıòîò ôàéë â ïàïêó include, êîòîğàÿ ëåæèò ğÿäîì ñ êîìïèëÿòîğîì
        - Â ñàìîì âåğõó âàøåãî ñêğèïòà äîáàâèòü #include <mxINI.inc>

    ÀÂÒÎĞ
         MX_Master

    ÂÅĞÑÈß
        0.5 îò 20.09.2010


    ÄÎÑÒÓÏÍÛÅ ÈÍÑÒĞÓÌÅÍÒÛ
    áîëåå ïîäğîáíûå îïèñàíèÿ äàíû ïåğåä êàæäîé ôóíêöèåé íèæå â êîäå

        ini_createFile ( "ïóòü/ê/ôàéëó.ini", "Ñîäåğæèìîå ôàéëà ïî óìîë÷àíèş" )

        ini_openFile ( "ïóòü/ê/ôàéëó.ini" )

        ini_closeFile ( ÈÄ_îòêğûòîãî_ôàéëà )
    --
        ini_setString ( ÈÄ_îòêğûòîãî_ôàéëà, "èìÿ êëş÷à", "òåêñòîâîå çíà÷åíèå" )

        ini_setInteger ( ÈÄ_îòêğûòîãî_ôàéëà, "èìÿ êëş÷à", 123456 )

        ini_setFloat ( ÈÄ_îòêğûòîãî_ôàéëà, "èìÿ êëş÷à", 3.1416 )
    --
        ini_getString ( ÈÄ_îòêğûòîãî_ôàéëà, "èìÿ êëş÷à", returnValue )

        ini_getInteger ( ÈÄ_îòêğûòîãî_ôàéëà, "èìÿ êëş÷à", returnValue )

        ini_getFloat ( ÈÄ_îòêğûòîãî_ôàéëà, "èìÿ êëş÷à", returnValue )
    --
        ini_removeKey ( ÈÄ_îòêğûòîãî_ôàéëà, "èìÿ êëş÷à" )

        ini_getErrorInfo ( Êîä_îøèáêè )


    ÏĞÈÌÅĞÛ

        new iniFile = ini_createFile ( "test4268.ini" );

        if ( iniFile < 0 )
            iniFile = ini_openFile ( "test4268.ini" );

        if ( iniFile >= 0 )
        {
            new returnString[32], returnNumber, Float: returnFloat;

            ini_setString  ( iniFile, "êëş÷ ñî ñòğîêîé", "òåêñòîâîå çíà÷åíèå" );
            ini_setInteger ( iniFile, "êëş÷ ñ ÷èñëîì",   123456 );
            ini_setFloat   ( iniFile, "êëş÷ ñ äğîáüş",   3.1416 );

            ini_removeKey  ( iniFile, "êëş÷ ñ ÷èñëîì" );

            ini_getString  ( iniFile, "êëş÷ ñî ñòğîêîé", returnString );
            ini_getInteger ( iniFile, "êëş÷ ñ ÷èñëîì",   returnNumber );
            ini_getFloat   ( iniFile, "êëş÷ ñ äğîáüş",   returnFloat );


            ini_closeFile  ( iniFile );

            printf ( "\n `êëş÷_ñî_ñòğîêîé` = `%s`,\n `êëş÷_ñ_÷èñëîì` = `%d`,\n `êëş÷_ñ_äğîáüş` = `%f` \n",
                returnString, returnNumber, returnFloat );
        }
        else print( "\n Íå óäàëîñü îòêğûòü INI ôàéë \n" );
*/




#define iniOpen ini_openFile
#define iniClose ini_closeFile
#define iniGet ini_getString
#define iniGetInt ini_getInteger
#define iniGetFloat ini_getFloat
#define iniSet ini_setString
#define iniSetInt ini_setInteger
#define iniSetFloat ini_setFloat
#define iniRemoveKey ini_removeKey






const

    // íàñòğîéêè

    // ĞÅÊÎÌÅÍÄÓÅÒÑß ÈÇÌÅÍÈÒÜ ÏÎÄ ÑÅÁß
    INI_MAX_OPENED_FILES    = 2,        // ìàêñèìóì, îòêğûòûõ îäíîâğåìåííî, ôàéëîâ
    INI_MAX_FILE_SIZE       = 65536,    // áàéò,        ìàêñ. ğàçìåğ ôàéëà
    INI_MAX_FILENAME_SIZE   = 128,      // ñèìâîëîâ,    ìàêñ. ğàçìåğ ïóòè ê ôàéëó
    INI_MAX_KEYS_IN_FILE    = 2048,     // ìàêñèìóì êëş÷åé â îòêğûòîì ôàéëå

    // â ÎÇÓ áóäåò âûäåëåíî äëÿ âğåìåííîãî õğàíåíèÿ ôàéëîâ ïğèìåğíî âîò ñòîëüêî áàéò:
    // INI_MAX_OPENED_FILES * ( INI_MAX_FILE_SIZE + INI_MAX_FILENAME_SIZE*4 + INI_MAX_KEYS_IN_FILE*4 )


    // ÍÅËÜÇß ÌÅÍßÒÜ
    INI_INTEGER_SIZE        = 12,       // ğàçìåğ ñòğîêè ñ öåëî÷èñëåííûì çíà÷åíèåì
    INI_FLOAT_SIZE          = 40,       // ğàçìåğ ñòğîêè ñ äğîáíûì ÷èñëîâûì çíà÷åíèåì

    INI_STRING_DELIMITER    = '\n',     // ğàçäåëèòåëü ñòğîê
    INI_DELIMITER           = '=',      // ğàçäåëèòåëü êëş÷à è çíà÷åíèÿ


    // êîäû îøèáîê, âîçâğàùàåìûå ôóíêöèÿìè

    // ĞÅÊÎÌÅÍÄÓÅÒÑß ÍÅ ÈÇÌÅÍßÒÜ
    INI_OK                  =  0,       // ôóíêöèÿ óñïåøíî âûïîëíåíà

        // ïğîâåğÿòü íà îøèáêó ìîæíî òàê:
        // if ( âîçâğàùàåìîå_çíà÷åíèå_ôóíêöèè < 0 ) ...

    INI_FILE_NOT_FOUND      = -1,       // ôàéë íå íàéäåí ïî óêàçàííîìó ïóòè
    INI_FILE_ALREADY_EXIST  = -2,       // ôàéë íå íàéäåí ïî óêàçàííîìó ïóòè
    INI_TOO_LARGE_FILE      = -3,       // ğàçìåğ ôàéëà ïğåâûñèë äîïóñòèìûé ëèìèò
    INI_WRONG_PATH_SIZE     = -4,       // íåïğàâèëüíûé ğàçìåğ ïóòè ê ôàéëó
    INI_READ_ERROR          = -5,       // îøèáêà ÷òåíèÿ ôàéëà
    INI_WRITE_ERROR         = -6,       // îøèáêà ïğè çàïèñè â ôàéë
    INI_NO_FREE_SLOT        = -7,       // íåò ñâîáîäíîãî ñëîòà äëÿ îòêğûòèÿ ôàéëà
    INI_WRONG_SLOT          = -8,       // óêàçàí íåâåğíûé ñëîò îòêğûòîãî ôàéëà
    INI_KEY_NOT_FOUND       = -9,       // êëş÷ â îòêğûòîì ôàéëå íå íàéäåí
    INI_WRONG_RETURN_SIZE   = -10,      // ğàçìåğ ñòğîêè, â êîòîğóş áóäåò ïîìåùåíî çíà÷åíèå êëş÷à - óêàçàí íåâåğíî (<= 0)


    // äğóãèå âñïîìîãàòåëüíûå êîíñòàíòû

    // ÍÅËÜÇß ÌÅÍßÒÜ
    cellbytes = cellbits / charbits;    // êîë-âî áàéò â îäíîé ÿ÷åéêå





// ñïèñêè ğàçíûõ ñèìâîëîâ äëÿ îïåğàòîğà case â îäíîé èç ôóíêöèé

// ĞÅÊÎÌÅÍÄÓÅÒÑß ÍÅ ÈÇÌÅÍßÒÜ
#define INI_SPACE_CHAR  ' ', '\t'                       // ñòğîêîâûå ïğîáåëüíûå ñèìâîëû
#define INI_KEY_STARTS  ' ', '\t', '\r', '\n', '\0'     // ñèìâîëû, ïåğåä íà÷àëîì êëş÷à
#define INI_STRING_ENDS '\r', '\n', '\0'                // ñèìâîëû, çàâåğøàşùèå çíà÷åíèå
#define INI_NUMBER_ENDS ' ', '\t', '\r', '\n', '\0'     // ñèìâîëû, çàâåğøàşùèå ÷èñëåííîå çíà÷åíèå





static stock

    // âğåìåííîå õğàíèëèùå îòêğûòûõ ôàéëîâ

    _ini_nSlotUsed      [ INI_MAX_OPENED_FILES ],                               // ôëàã: çàíÿò ëè ñëîò
    _ini_nFileChanged   [ INI_MAX_OPENED_FILES ],                               // ôëàã: áûë ëè èçìåíåí ôàéë
    _ini_nFileBusy      [ INI_MAX_OPENED_FILES ],                               // ôëàã: èçìåíÿåòñÿ ëè â äàííûé ìîìåíò ñîäåğæèìîå ôàéëà
    _ini_nFileSize      [ INI_MAX_OPENED_FILES ],                               // ğàçìåğ îòêğûòîãî ôàéëà
    _ini_nDelimPos      [ INI_MAX_OPENED_FILES ] [ INI_MAX_KEYS_IN_FILE ],      // ñïèñîê ïîçèöèé INI_DELIMITER
    _ini_nKeysCount     [ INI_MAX_OPENED_FILES ],                               // êîë-âî êëş÷åé îòêğûòîãî ôàéëà
    _ini_szFilePath     [ INI_MAX_OPENED_FILES ] [ INI_MAX_FILENAME_SIZE ],     // ïóòü ê ôàéëó
    _ini_szFileContent  [ INI_MAX_OPENED_FILES ] [ INI_MAX_FILE_SIZE char ];    // êîíòåíò ôàéëà





















/*
    Ñîçäàåò è ñğàçó îòêğûâàåò INI ôàéë äëÿ ÷òåíèÿ/çàïèñè.

    ÏÎÄĞÎÁÍÅÅ
        Ôàéë ñîçäàåòñÿ òîëüêî â ÎÇÓ, è â íåãî çàïèñûâàåòñÿ ñòğîêà szDefaultContent.
        Â szDefaultContent ìîãóò áûòü è êëş÷è, êîòîğûå ïîòîì ïàğñåğ òàêæå áóäåò âèäåòü.
        Òîëüêî ïğè çàêğûòèè ôàéëà, åãî ñîäåğæèìîå áóäåò çàïèñàíî íà äèñê
        ïî óêàçàííîìó ïóòè ê ôàéëó.

    ÏÀĞÀÌÅÒĞÛ:
        szFilePath[]        ïóòü ê ôàéëó
        szDefaultContent    êîíòåíò ôàéëà ïî óìîë÷àíèş, îáû÷íî, ìîæíî íå óêàçûâàòü

    ÂÅĞÍÅÒ:
        êîä îøèáêè < 0 èëè ÈÄ_îòêğûòîãî_ôàéëà
*/

stock ini_createFile ( szFilePath[], szDefaultContent[] = "" )
{
    //
    //  íåñêîëüêî áëîêîâ ñ ïğîâåğêàìè
    //

    // ------------
    new nFileNameSize = strlen( szFilePath ); // óçíàåì ğàçìåğ ïóòè

    // åñëè ğàçìåğ ïóòè íåïğàâèëüíûé
    if ( nFileNameSize <= 0 || nFileNameSize >= INI_MAX_FILENAME_SIZE )
        return INI_WRONG_PATH_SIZE; // âåğíåì êîä îøèáêè
    // ------------


    // ------------
    if ( fexist( szFilePath ) ) // åñëè ôàéë óæå ñóùåñòâóåò
        return INI_FILE_ALREADY_EXIST; // âåğíåì êîä îøèáêè - ôàéë óæå ñóùåñòâóåò

    for ( new slot = 0; slot < INI_MAX_OPENED_FILES; slot++ ) // ïåğåáîğ âñåõ ñëîòîâ ÎÇÓ
        if // åñëè óæå åñòü òàêîé îòêğûòûé ôàéë
        (
                _ini_nSlotUsed[slot] != 0
            &&  strcmp( szFilePath, _ini_szFilePath[slot], false ) == 0
        )
            return INI_FILE_ALREADY_EXIST; // âåğíåì êîä îøèáêè - ôàéë óæå ñóùåñòâóåò
    // ------------



    // ------------
    new nFileSize = strlen( szDefaultContent ); // óçíàåì ğàçìåğ êîíòåíòa ôàéëà ïî óìîë÷àíèş

    // åñëè ğàçìåğ íåïğàâèëüíûé
    if ( nFileSize < 0 || nFileSize >= INI_MAX_FILE_SIZE )
        return INI_TOO_LARGE_FILE; // âåğíåì êîä îøèáêè
    // ------------




    //
    //  ïîèñê ñâîáîäíîãî ñëîòà äëÿ çàïèñè â ÎÇÓ
    //

    for ( new slot = 0; slot < INI_MAX_OPENED_FILES; slot++ ) // ïåğåáîğ âñåõ ñëîòîâ ÎÇÓ
    {
        if ( _ini_nSlotUsed[slot] != 1 ) // åñëè ñëîò íàéäåí
        {
            // ------------
            _ini_nSlotUsed[slot]        = 1; // çàñòîëáèì íàéäåííîå ìåñòî
            _ini_nFileChanged[slot]     = 0; // ôàéë íå áûë èçìåíåí
            _ini_nFileSize[slot]        = nFileSize; // ñêîïèğóåì ğàçìåğ ôàéëà â ñëîò
            _ini_nKeysCount[slot]       = 0; // êîë-âî êëş÷åé âûñòàâèì 0
            // ------------


            // ------------
            // ñêîïèğóåì â ÎÇÓ âåñü ôàéë,
            // çàïîìíèâ âñå ïîçèöèè INI_DELIMITER è èõ êîë-âî
            for ( new i = 0;  i < nFileSize && i < INI_MAX_FILE_SIZE;  i++ )
            {
                if (
                        szDefaultContent[i] == INI_DELIMITER // åñëè ñèìâîë ıòî INI_DELIMITER
                    &&  _ini_nKeysCount[slot] < INI_MAX_KEYS_IN_FILE // è ëèìèò êëş÷åé åùå íå èñ÷åğïàí
                ) {
                    _ini_nDelimPos[slot][ _ini_nKeysCount[slot] ] = i; // äîáàâèì åùå îäíó ïîçèöèş INI_DELIMITER
                    _ini_nKeysCount[slot]++; // óâåëè÷èì êîë-âî íàéäåííûõ êëş÷åé
                }

                _ini_szFileContent[slot]{i} = szDefaultContent[i];
            }

            _ini_szFileContent[slot]{nFileSize} = 0; // ñèìâîë êîíöà ñòğîêè äëÿ êîíòåíòà
            // ------------


            // ------------
            // ñêîïèğóåì â îçó ïóòü ê ôàéëó
            memcpy( _ini_szFilePath[slot], szFilePath, 0, nFileNameSize * cellbytes );

            _ini_szFilePath[slot][nFileNameSize] = 0; // ñèìâîë êîíöà ñòğîêè äëÿ ïóòè
            // ------------


            return slot; // âåğíåì ÈÄ ñëîòà
        }
    }




    //
    //  åñëè ñâîáîäíûé ñëîò â ïàìÿòè íå íàéäåí
    //

    return INI_NO_FREE_SLOT; // âåğíåì êîä îøèáêè
    // ------------
}




/*
    Îòêğûâàåò INI ôàéë äëÿ ÷òåíèÿ/çàïèñè, åñëè îí ñóùåñòâóåò.

    ÏÎÄĞÎÁÍÅÅ
        Êîïèğóåò â ÎÇÓ ñ äèñêà âñ¸ ñîäåğæèìîå ôàéëà, åñëè åãî ğàçìåğ íå ïğåâûøàåò
        äîïóñòèìûé. Åñëè êîë-âî êëş÷åé â ôàéëå áîëüøå äîïóñòèìîãî, îøèáêè íèêàêîé íå áóäåò,
        ïğîñòî ïğè ÷òåíèè/çàïèñè çíà÷åíèé, ïàğñåğ íå áóäåò âèäåòü ëèøíèå êëş÷è.

    ÏÀĞÀÌÅÒĞÛ:
        szFilePath[]    ïóòü ê ôàéëó

    ÂÅĞÍÅÒ:
        êîä îøèáêè < 0 èëè ÈÄ_îòêğûòîãî_ôàéëà
*/

stock ini_openFile ( szFilePath[] )
{
    //
    //  íåñêîëüêî áëîêîâ ñ ïğîâåğêàìè
    //

    // ------------
    new nFileNameSize = strlen( szFilePath ); // óçíàåì ğàçìåğ ïóòè

    // åñëè ğàçìåğ ïóòè íåïğàâèëüíûé
    if ( nFileNameSize <= 0 || nFileNameSize >= INI_MAX_FILENAME_SIZE )
        return INI_WRONG_PATH_SIZE; // âåğíåì êîä îøèáêè
    // ------------


    if ( ! fexist( szFilePath ) ) // åñëè ôàéë íå íàéäåí
        return INI_FILE_NOT_FOUND; // âåğíåì êîä îøèáêè


    // ------------
    // ïğîâåğêà - îòêğûò ëè óæå ôàéë ñ òàêèì èìåíåì
    for ( new slot = 0; slot < INI_MAX_OPENED_FILES; slot++ ) // ïåğåáîğ âñåõ ñëîòîâ ÎÇÓ
        if // åñëè óæå åñòü òàêîé îòêğûòûé ôàéë
        (
                _ini_nSlotUsed[slot] != 0
            &&  strcmp( szFilePath, _ini_szFilePath[slot], false ) == 0
        )
            return slot; // ïğîñòî âåğíåì åãî ñëîò
    // ------------


    // ------------
    new File: pFile = fopen( szFilePath, io_read ); // ïûòàåìñÿ îòêğûòü ôàéë äëÿ ÷òåíèÿ

    if ( ! pFile ) // åñëè ôàéë íå îòêğûëñÿ
        return INI_READ_ERROR; // âåğíåì êîä îøèáêè
    // ------------


    // ------------
    new nFileSize = flength( pFile ); // óçíàåì ğàçìåğ ôàéëà

    if ( nFileSize >= INI_MAX_FILE_SIZE ) // åñëè ğàçìåğ ôàéëà ñëèøêîì áîëüøîé
    {
        fclose(pFile); // çàêğîåì ôàéë
        return INI_TOO_LARGE_FILE; // âåğíåì êîä îøèáêè
    }
    // ------------




    //
    //  ïîèñê ñâîáîäíîãî ñëîòà äëÿ çàïèñè â ÎÇÓ
    //

    // ------------
    for ( new slot = 0; slot < INI_MAX_OPENED_FILES; slot++ ) // ïåğåáîğ âñåõ ñëîòîâ ÎÇÓ
    {
        if ( _ini_nSlotUsed[slot] != 1 ) // åñëè ñëîò íàéäåí
        {
            // ------------
            _ini_nSlotUsed[slot]        = 1; // çàñòîëáèì íàéäåííîå ìåñòî
            _ini_nFileChanged[slot]     = 0; // ôàéë íå áûë èçìåíåí
            _ini_nFileSize[slot]        = nFileSize; // ñêîïèğóåì ğàçìåğ ôàéëà â ñëîò
            _ini_nKeysCount[slot]       = 0; // êîë-âî êëş÷åé âûñòàâèì 0
            // ------------


            // ------------
            // ñêîïèğóåì â ÎÇÓ âåñü ôàéë,
            // çàïîìíèâ âñå ïîçèöèè INI_DELIMITER è èõ êîë-âî
            for ( new i = 0, symbol; i < nFileSize && i < INI_MAX_FILE_SIZE; i++ )
            {
                symbol = fgetchar( pFile, 0, false ); // ÷èòàåì èç ôàéëà ñëåäóşùèé ñèìâîë

                if (
                        symbol == INI_DELIMITER // åñëè ñèìâîë ıòî INI_DELIMITER
                    &&  _ini_nKeysCount[slot] < INI_MAX_KEYS_IN_FILE // è ëèìèò êëş÷åé åùå íå èñ÷åğïàí
                ) {
                    _ini_nDelimPos[slot][ _ini_nKeysCount[slot] ] = i; // äîáàâèì åùå îäíó ïîçèöèş INI_DELIMITER
                    _ini_nKeysCount[slot]++; // óâåëè÷èì êîë-âî íàéäåííûõ êëş÷åé
                }

                _ini_szFileContent[slot]{i} = symbol;
            }

            _ini_szFileContent[slot]{nFileSize} = 0; // ñèìâîë êîíöà ñòğîêè äëÿ êîíòåíòà

            fclose(pFile); // çàêğîåì ôàéë
            // ------------


            // ------------
            // ñêîïèğóåì â îçó ïóòü ê ôàéëó
            memcpy( _ini_szFilePath[slot], szFilePath, 0, nFileNameSize * cellbytes );

            _ini_szFilePath[slot][nFileNameSize] = 0; // ñèìâîë êîíöà ñòğîêè äëÿ ïóòè
            // ------------


            return slot; // âåğíåì ÈÄ ñëîòà
        }
    }
    // ------------




    //
    //  åñëè ñâîáîäíûé ñëîò â ïàìÿòè íå íàéäåí
    //

    // ------------
    fclose(pFile); // çàêğîåì ôàéë

    return INI_NO_FREE_SLOT; // âåğíåì êîä îøèáêè
    // ------------
}




/*
    Çàêğûâàåò INI ôàéë, åñëè îí áûë îòêğûò.

    ÏÎÄĞÎÁÍÅÅ
        Åñëè ôàéë íå áûë èçìåíåí - îñâîáîäèò ñëîò äëÿ õğàíåíèÿ ôàéëà â ÎÇÓ.
        Åñëè ôàéë áûë èçìåíåí - ïîëíîñòüş ïåğåçàïèøåò ôàéë íà äèñêå.

    ÏÀĞÀÌÅÒĞÛ:
        nFilePointer    ÈÄ_îòêğûòîãî_ôàéëà, ïîëó÷åííûé îò ini_openFile / ini_createFile

    ÂÅĞÍÅÒ:
        êîä îøèáêè < 0 èëè 0 ïğè óñïåõå
*/

stock ini_closeFile ( nFilePointer )
{
    if
    (   // åñëè ÈÄ îòêğûòîãî ôàéëà óêàçàí âåğíî
            nFilePointer >= 0
        &&  nFilePointer < INI_MAX_OPENED_FILES
        &&  _ini_nSlotUsed[nFilePointer] != 0
    )
    {
        if ( _ini_nFileChanged[nFilePointer] != 0 ) // åñëè ôàéë áûë èçìåíåí
        {
            new File: pFile = fopen( _ini_szFilePath[nFilePointer], io_write ); // ïûòàåìñÿ îòêğûòü ôàéë äëÿ çàïèñè

            if ( ! pFile ) // åñëè ôàéë íå îòêğûëñÿ
                return INI_WRITE_ERROR; // âåğíåì êîä îøèáêè

            // çàïèøåì êîíòåíò ôàéëà èç ÎÇÓ íà äèñê
            for ( new i = 0;  i < _ini_nFileSize[nFilePointer];  i++ )
                fputchar( pFile, _ini_szFileContent[nFilePointer]{i}, false );

            fclose(pFile); // çàêğîåì ôàéë
        }

        _ini_nSlotUsed[nFilePointer] = 0; // îñâîáîäèòü ñëîò îòêğûòîãî ôàéëà

        return INI_OK; // âåğíóòü êîä îá óñïåøíîì âûïîëíåíèè ôóíêöèè
    }

    return INI_WRONG_SLOT; // âåğíóòü êîä: íåâåğíûé óêàçàòåëü íà îòêğûòûé ôàéë
}











/*
    Ïîëó÷àåò èç îòêğûòîãî INI ôàéëà çíà÷åíèå óêàçàííîãî êëş÷à.

    ÏÎÄĞÎÁÍÅÅ
        Ïàğñåğ èùåò â ÎÇÓ â êîíòåíòå ôàéëà óêàçàííûé êëş÷ è ïîìåùàåò â
        szReturnValue åãî ñòğîêîâîå çíà÷åíèå. szReturnValue íóæíî ñîçäàòü çàğàíåå.

        nSizeOfReturn ìîæíî íå óêàçûâàòü, åñëè òîëüêî íå íóæíî òî÷íîå êîë-âî
        âîçâğàùàåìûõ ñèìâîëîâ â ñòğîêå (â îïèñàíèè ıòîãî ïàğàìåòğà íèæå äàíû
        äîïîëíèòåëüíûå óêàçàíèÿ).

        Êàê êëş÷ òàê è çíà÷åíèå â ôàéëå, ìîãóò áûòü ëşáîé äëèíû è
        ìîãóò ñîäåğæàòü ëşáûå ñèìâîëû, êğîìå 2 ñèìâîëîâ êîíöà ñòğîêè \r è \n.

        Ïàğñåğ íå âèäèò ïğîáåëû è çíàêè òàáóëÿöèè âîêğóã èìåíè êëş÷à è
        ïåğåä çíà÷åíèåì, îí ñ÷èòàåò èõ îòñòóïàìè.

    ÏÀĞÀÌÅÒĞÛ:
        nFilePointer        ÈÄ_îòêğûòîãî_ôàéëà, ïîëó÷åííûé îò ini_openFile / ini_createFile
        szKeyName[]         èìÿ êëş÷à
        szReturnValue[]     ñşäà áóäåò ïîìåùåíî çíà÷åíèå êëş÷à â âèäå ñòğîêè
        nSizeOfReturn       ìàêñ. ğàçìåğ âîçâğàùàåìîé ñòğîêè, îáû÷íî, ıòî ğàçìåğ szReturnValue.
                            åñëè âàøà ñòğîêà szReturnValue ÿâëÿåòñÿ ÷àñòüş ìàññèâà, êîòîğûé áûë ñîçäàí ñ ïîìîùüş enum,
                            ıòîò ïàğàìåòğ íóæíî îáÿçàòåëüíî óêàçûâàòü êàê ÷èñëî

    ÂÅĞÍÅÒ:
        êîä îøèáêè < 0 èëè 0 ïğè óñïåõå
*/

stock ini_getString ( nFilePointer, szKeyName[], szReturnValue[], nSizeOfReturn = sizeof(szReturnValue) )
{
    // ----------------
    if // åñëè ÈÄ îòêğûòîãî ôàéëà óêàçàí íåâåğíî
    (
            nFilePointer < 0
        ||  nFilePointer >= INI_MAX_OPENED_FILES
        ||  _ini_nSlotUsed[nFilePointer] != 1
    )
        return INI_WRONG_SLOT; // âåğíóòü êîä: íåâåğíûé óêàçàòåëü íà îòêğûòûé ôàéë
    // ----------------


    if ( nSizeOfReturn <= 0 ) // åñëè ïî êàêîé-òî ïğè÷èíå ğàçìåğ âîçâğàùàåìîãî çíà÷åíèÿ óêàçàí/ğàññ÷èòàí êàê 0
        return INI_WRONG_RETURN_SIZE;


    // ----------------
    new nKeyLen = strlen(szKeyName); // óçíàåì äëèíó èìåíè óêàçàííîãî êëş÷à

    if ( nKeyLen <= 0 ) // åñëè óêàçàí ïóñòîé êëş÷
        return INI_KEY_NOT_FOUND;
    // ----------------


    // ----------------
    for // ïåğåáîğ è ñğàâíåíèå âñåõ êëş÷åé ôàéëà ñ óêàçàííûì êëş÷îì
    (
        new kPos = 0, curFilePos, found = 0;
        kPos < _ini_nKeysCount[nFilePointer];
        kPos++
    )
    {
        // ----------------
        found = 0; // ôëàã, íàéäåíà ëè ïîçèöèÿ êîíöà êëş÷à = 0

        for // èùåì ïîçèöèş êîíöà êëş÷à
        (
            curFilePos = _ini_nDelimPos[nFilePointer][kPos] - 1; // òåêóù. ïîç. ôàéëà = ïîç. òåêóù. INI_DELIMITER - 1
            curFilePos >= 0; // ïğîäîëæàòü ïîêà ïîç. ôàéëà >= 0
            curFilePos-- // ïîñëå êàæäîãî ïîâòîğà òåêóù. ïîç. ôàéëà -= 1
        )
        {
            switch ( _ini_szFileContent[nFilePointer]{curFilePos} ) // óçíàåì ÷òî çà ñèìâîë â òåêóù. ïîç. ôàéëà
            {
                case INI_SPACE_CHAR :           continue; // åñëè ıòî ïğîáåëüíûé ñèìâîë, ïåğåéäåì ê ïğåä. ñèìâîëó ôàéëà
                case INI_STRING_DELIMITER :     break; // åñëè ıòî êîíåö ñòğîêè
                default : // åñëè ıòî äğóãîé ñèìâîë
                {
                    found = 1; // ïîçèöèÿ êîíöà êëş÷à íàéäåíà
                    break; // êîíåö öèêëà
                }
            }
        }

        // åñëè ïîçèöèÿ êîíöà êëş÷à íå íàéäåíà, ïåğåõîä ê ñëåä. ïîçèöèè INI_DELIMITER
        if ( found != 1 ) continue;
        // ----------------


        // ñğàâíèâàåì ïîñèìâîëüíî òåêóùèé êëş÷ ôàéëà è óêàçàííûé êëş÷
        for ( new curKeyPos = nKeyLen - 1;  curKeyPos >= 0;  curFilePos--, curKeyPos-- )
        {
            if
            (
                    curFilePos < 0 // åñëè ïîç ôàéëà ñòàëà < 0
                ||  _ini_szFileContent[nFilePointer]{curFilePos} != szKeyName[curKeyPos] // åñëè ñèìâîëû èç êëş÷åé íå ğàâíû
                ||  _ini_szFileContent[nFilePointer]{curFilePos} == INI_STRING_DELIMITER // åñëè ñèìâîë èç êëş÷à ıòî INI_STRING_DELIMITER
            )
            {
                found = 0; // ôëàã, êëş÷ íå íàéäåí
                break; // êîíåö ñğàâíåíèÿ
            }
        }


        // ----------------
        if ( found != 0 ) // åñëè óêàçàííûé êëş÷ íàéäåí â ôàéëå
        {
            // åñëè íàéäåíî ñîâïàäåíèå íå öåëîãî êëş÷à ôàéëà, à åãî îêîí÷àíèÿ ñ óêàçàííûì êëş÷îì
            if ( curFilePos >= 0 )
            {
                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_KEY_STARTS : {}
                    default: continue;
                }
            }


            // ----------------
            // òåêóùàÿ ïîçèöèÿ â ôàéëå áóäåò íà 1 áîëüøå òåêóùåé ïîçèöèè INI_DELIMITER
            curFilePos = _ini_nDelimPos[nFilePointer][kPos] + 1;

            // èùåì ïîçèöèş íà÷àëà çíà÷åíèÿ, îíà áóäåò ïîìåùåíà â curFilePos
            for ( ; ; curFilePos++ )
            {
                if ( curFilePos >= _ini_nFileSize[nFilePointer] ) break;

                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_SPACE_CHAR :   continue; // åñëè ıòî ïğîáåëüíûé ñèìâîë, ïåğåéäåì ê ñëåä. ñèìâîëó
                    default :               break; // åñëè ıòî äğóãîé ñèìâîë
                }
            }
            // ----------------


            // ----------------
            // ñêîïèğóåì ïîñèìâîëüíî â szReturnValue çíà÷åíèå êëş÷à èç ôàéëà
            // âîñïîëüçóåìñÿ ñîçäàííîé ïåğåìåííîé found êàê ïîçèöèåé â âîçâğàùàåìîì çíà÷åíèè
            for ( found = 0;  found < nSizeOfReturn;  found++, curFilePos++ )
            {
                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_STRING_ENDS : // åñëè ıòî ñèìâîë êîíöà çíà÷åíèÿ
                    {
                        szReturnValue[found] = 0; // çàïèøåì ñèìâîë êîíöà ñòğîêè
                        break; // êîíåö êîïèğîâàíèÿ
                    }
                    default :
                        // êîïèğóåì ñèìâîë èç ôàéëà â szReturnValue
                        szReturnValue[found] = _ini_szFileContent[nFilePointer]{curFilePos};
                }
            }

            szReturnValue[nSizeOfReturn - 1] = 0; // íà âñÿêèé ñëó÷àé îáğåæåì ïğàâèëüíî ñòğîêó

            return INI_OK;
            // ----------------
        }
        // ----------------
    }
    // ----------------


    // ----------------
    return INI_KEY_NOT_FOUND;
    // ----------------
}




/*
    Ïîëó÷àåò èç îòêğûòîãî INI ôàéëà öåëî÷èñëåííîå çíà÷åíèå óêàçàííîãî êëş÷à.

    ÏÎÄĞÎÁÍÅÅ
        Ïàğñåğ èùåò â ÎÇÓ â êîíòåíòå ôàéëà óêàçàííûé êëş÷ è ïîìåùàåò â
        nReturnValue åãî öåëî÷èñëåííîå çíà÷åíèå. nReturnValue íóæíî ñîçäàòü çàğàíåå.

        Èìÿ êëş÷à â ôàéëå ìîæåò áûòü ëşáîé äëèíû, à òàêæå ìîæåò ñîäåğæàòü
        ëşáûå ñèìâîëû, êğîìå 2 ñèìâîëîâ êîíöà ñòğîêè \r è \n.

        Çíà÷åíèå ìîæåò áûòü òîëüêî ôèêñèğîâàííîé äëèíû, êîòîğàÿ ğàâíà INI_INTEGER_SIZE - 1.
        Ïàğñåğ íå âèäèò ïğîáåëû è çíàêè òàáóëÿöèè âîêğóã èìåíè êëş÷à/çíà÷åíèÿ,
        îí ñ÷èòàåò èõ îòñòóïàìè.

    ÏÀĞÀÌÅÒĞÛ:
        nFilePointer        ÈÄ_îòêğûòîãî_ôàéëà, ïîëó÷åííûé îò ini_openFile / ini_createFile
        szKeyName[]         èìÿ êëş÷à
        nReturnValue        ñşäà áóäåò ïîìåùåíî çíà÷åíèå êëş÷à â âèäå öåëîãî ÷èñëà

    ÂÅĞÍÅÒ:
        êîä îøèáêè < 0 èëè 0 ïğè óñïåõå
*/

stock ini_getInteger ( nFilePointer, szKeyName[], & nReturnValue )
{
    // ----------------
    if // åñëè ÈÄ îòêğûòîãî ôàéëà óêàçàí íåâåğíî
    (
            nFilePointer < 0
        ||  nFilePointer >= INI_MAX_OPENED_FILES
        ||  _ini_nSlotUsed[nFilePointer] != 1
    )
        return INI_WRONG_SLOT; // âåğíóòü êîä: íåâåğíûé óêàçàòåëü íà îòêğûòûé ôàéë
    // ----------------


    // ----------------
    new nKeyLen = strlen(szKeyName); // óçíàåì äëèíó èìåíè óêàçàííîãî êëş÷à

    if ( nKeyLen <= 0 ) // åñëè óêàçàí ïóñòîé êëş÷
        return INI_KEY_NOT_FOUND;
    // ----------------


    // ----------------
    for // ïåğåáîğ è ñğàâíåíèå âñåõ êëş÷åé ôàéëà ñ óêàçàííûì êëş÷îì
    (
        new kPos = 0, curFilePos, found = 0;
        kPos < _ini_nKeysCount[nFilePointer];
        kPos++
    )
    {
        // ----------------
        found = 0; // ôëàã, íàéäåíà ëè ïîçèöèÿ êîíöà êëş÷à = 0

        for // èùåì ïîçèöèş êîíöà êëş÷à
        (
            curFilePos = _ini_nDelimPos[nFilePointer][kPos] - 1; // òåêóù. ïîç. ôàéëà = ïîç. òåêóù. INI_DELIMITER - 1
            curFilePos >= 0; // ïğîäîëæàòü ïîêà ïîç. ôàéëà >= 0
            curFilePos-- // ïîñëå êàæäîãî ïîâòîğà òåêóù. ïîç. ôàéëà -= 1
        )
        {
            switch ( _ini_szFileContent[nFilePointer]{curFilePos} ) // óçíàåì ÷òî çà ñèìâîë â òåêóù. ïîç. ôàéëà
            {
                case INI_SPACE_CHAR :           continue; // åñëè ıòî ïğîáåëüíûé ñèìâîë, ïåğåéäåì ê ïğåä. ñèìâîëó ôàéëà
                case INI_STRING_DELIMITER :     break; // åñëè ıòî êîíåö ñòğîêè
                default : // åñëè ıòî äğóãîé ñèìâîë
                {
                    found = 1; // ïîçèöèÿ êîíöà êëş÷à íàéäåíà
                    break; // êîíåö öèêëà
                }
            }
        }

        // åñëè ïîçèöèÿ êîíöà êëş÷à íå íàéäåíà, ïåğåõîä ê ñëåä. ïîçèöèè INI_DELIMITER
        if ( found != 1 ) continue;
        // ----------------


        // ñğàâíèâàåì ïîñèìâîëüíî òåêóùèé êëş÷ ôàéëà è óêàçàííûé êëş÷
        for ( new curKeyPos = nKeyLen - 1;  curKeyPos >= 0;  curFilePos--, curKeyPos-- )
        {
            if
            (
                    curFilePos < 0 // åñëè ïîç ôàéëà ñòàëà < 0
                ||  _ini_szFileContent[nFilePointer]{curFilePos} != szKeyName[curKeyPos] // åñëè ñèìâîëû èç êëş÷åé íå ğàâíû
                ||  _ini_szFileContent[nFilePointer]{curFilePos} == INI_STRING_DELIMITER // åñëè ñèìâîë èç êëş÷à ıòî INI_STRING_DELIMITER
            )
            {
                found = 0; // ôëàã, êëş÷ íå íàéäåí
                break; // êîíåö ñğàâíåíèÿ
            }
        }


        // ----------------
        if ( found != 0 ) // åñëè óêàçàííûé êëş÷ íàéäåí â ôàéëå
        {
            // åñëè íàéäåíî ñîâïàäåíèå íå öåëîãî êëş÷à ôàéëà, à åãî îêîí÷àíèÿ ñ óêàçàííûì êëş÷îì
            if ( curFilePos >= 0 )
            {
                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_KEY_STARTS : {}
                    default: continue;
                }
            }


            // ----------------
            // òåêóùàÿ ïîçèöèÿ â ôàéëå áóäåò íà 1 áîëüøå òåêóùåé ïîçèöèè INI_DELIMITER
            curFilePos = _ini_nDelimPos[nFilePointer][kPos] + 1;

            // èùåì ïîçèöèş íà÷àëà çíà÷åíèÿ, îíà áóäåò ïîìåùåíà â curFilePos
            for ( ; ; curFilePos++ )
            {
                if ( curFilePos >= _ini_nFileSize[nFilePointer] ) break;

                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_SPACE_CHAR :   continue; // åñëè ıòî ïğîáåëüíûé ñèìâîë, ïåğåéäåì ê ñëåä. ñèìâîëó
                    default :               break; // åñëè ıòî äğóãîé ñèìâîë
                }
            }
            // ----------------


            // ----------------
            new strValue[INI_INTEGER_SIZE]; // âğåìåííàÿ ñòğîêà äëÿ ÷èñëåííîãî çíà÷åíèÿ

            // ñêîïèğóåì ïîñèìâîëüíî â strValue çíà÷åíèå êëş÷à èç ôàéëà
            // âîñïîëüçóåìñÿ ñîçäàííîé ïåğåìåííîé found êàê ïîçèöèåé â âîçâğàùàåìîì çíà÷åíèè
            for ( found = 0;  found < INI_INTEGER_SIZE;  found++, curFilePos++ )
            {
                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_NUMBER_ENDS : // åñëè ıòî ñèìâîë êîíöà ÷èñëåííîãî çíà÷åíèÿ
                    {
                        strValue[found] = 0; // çàïèøåì ñèìâîë êîíöà ñòğîêè
                        break; // êîíåö êîïèğîâàíèÿ
                    }
                    default :
                        // êîïèğóåì ñèìâîë èç ôàéëà â strValue
                        strValue[found] = _ini_szFileContent[nFilePointer]{curFilePos};
                }
            }

            strValue[INI_INTEGER_SIZE - 1] = 0; // íà âñÿêèé ñëó÷àé îáğåæåì ïğàâèëüíî ñòğîêó

            nReturnValue = strval(strValue); // çàïèøåì â nReturnValue ÷èñëåííîå çíà÷åíèå êëş÷à

            return INI_OK;
            // ----------------
        }
        // ----------------
    }
    // ----------------


    // ----------------
    return INI_KEY_NOT_FOUND;
    // ----------------
}




/*
    Ïîëó÷àåò èç îòêğûòîãî INI ôàéëà äğîáíîå ÷èñëåííîå çíà÷åíèå óêàçàííîãî êëş÷à.

    ÏÎÄĞÎÁÍÅÅ
        Ïàğñåğ èùåò â ÎÇÓ â êîíòåíòå ôàéëà óêàçàííûé êëş÷ è ïîìåùàåò â
        fReturnValue åãî äğîáíîå çíà÷åíèå. fReturnValue íóæíî ñîçäàòü çàğàíåå.

        Èìÿ êëş÷à â ôàéëå ìîæåò áûòü ëşáîé äëèíû, à òàêæå ìîæåò ñîäåğæàòü
        ëşáûå ñèìâîëû, êğîìå 2 ñèìâîëîâ êîíöà ñòğîêè \r è \n.

        Çíà÷åíèå ìîæåò áûòü òîëüêî ôèêñèğîâàííîé äëèíû, êîòîğàÿ ğàâíà INI_FLOAT_SIZE - 1.
        Ïàğñåğ íå âèäèò ïğîáåëû è çíàêè òàáóëÿöèè âîêğóã èìåíè êëş÷à/çíà÷åíèÿ,
        îí ñ÷èòàåò èõ îòñòóïàìè.

    ÏÀĞÀÌÅÒĞÛ:
        nFilePointer        ÈÄ_îòêğûòîãî_ôàéëà, ïîëó÷åííûé îò ini_openFile / ini_createFile
        szKeyName[]         èìÿ êëş÷à
        fReturnValue        ñşäà áóäåò ïîìåùåíî çíà÷åíèå êëş÷à â âèäå äğîáíîãî ÷èñëà

    ÂÅĞÍÅÒ:
        êîä îøèáêè < 0 èëè 0 ïğè óñïåõå
*/

stock ini_getFloat ( nFilePointer, szKeyName[], & Float: fReturnValue )
{
    // ----------------
    if // åñëè ÈÄ îòêğûòîãî ôàéëà óêàçàí íåâåğíî
    (
            nFilePointer < 0
        ||  nFilePointer >= INI_MAX_OPENED_FILES
        ||  _ini_nSlotUsed[nFilePointer] != 1
    )
        return INI_WRONG_SLOT; // âåğíóòü êîä: íåâåğíûé óêàçàòåëü íà îòêğûòûé ôàéë
    // ----------------


    // ----------------
    new nKeyLen = strlen(szKeyName); // óçíàåì äëèíó èìåíè óêàçàííîãî êëş÷à

    if ( nKeyLen <= 0 ) // åñëè óêàçàí ïóñòîé êëş÷
        return INI_KEY_NOT_FOUND;
    // ----------------


    // ----------------
    for // ïåğåáîğ è ñğàâíåíèå âñåõ êëş÷åé ôàéëà ñ óêàçàííûì êëş÷îì
    (
        new kPos = 0, curFilePos, found = 0;
        kPos < _ini_nKeysCount[nFilePointer];
        kPos++
    )
    {
        // ----------------
        found = 0; // ôëàã, íàéäåíà ëè ïîçèöèÿ êîíöà êëş÷à = 0

        for // èùåì ïîçèöèş êîíöà êëş÷à
        (
            curFilePos = _ini_nDelimPos[nFilePointer][kPos] - 1; // òåêóù. ïîç. ôàéëà = ïîç. òåêóù. INI_DELIMITER - 1
            curFilePos >= 0; // ïğîäîëæàòü ïîêà ïîç. ôàéëà >= 0
            curFilePos-- // ïîñëå êàæäîãî ïîâòîğà òåêóù. ïîç. ôàéëà -= 1
        )
        {
            switch ( _ini_szFileContent[nFilePointer]{curFilePos} ) // óçíàåì ÷òî çà ñèìâîë â òåêóù. ïîç. ôàéëà
            {
                case INI_SPACE_CHAR :           continue; // åñëè ıòî ïğîáåëüíûé ñèìâîë, ïåğåéäåì ê ïğåä. ñèìâîëó ôàéëà
                case INI_STRING_DELIMITER :     break; // åñëè ıòî êîíåö ñòğîêè
                default : // åñëè ıòî äğóãîé ñèìâîë
                {
                    found = 1; // ïîçèöèÿ êîíöà êëş÷à íàéäåíà
                    break; // êîíåö öèêëà
                }
            }
        }

        // åñëè ïîçèöèÿ êîíöà êëş÷à íå íàéäåíà, ïåğåõîä ê ñëåä. ïîçèöèè INI_DELIMITER
        if ( found != 1 ) continue;
        // ----------------


        // ñğàâíèâàåì ïîñèìâîëüíî òåêóùèé êëş÷ ôàéëà è óêàçàííûé êëş÷
        for ( new curKeyPos = nKeyLen - 1;  curKeyPos >= 0;  curFilePos--, curKeyPos-- )
        {
            if
            (
                    curFilePos < 0 // åñëè ïîç ôàéëà ñòàëà < 0
                ||  _ini_szFileContent[nFilePointer]{curFilePos} != szKeyName[curKeyPos] // åñëè ñèìâîëû èç êëş÷åé íå ğàâíû
                ||  _ini_szFileContent[nFilePointer]{curFilePos} == INI_STRING_DELIMITER // åñëè ñèìâîë èç êëş÷à ıòî INI_STRING_DELIMITER
            )
            {
                found = 0; // ôëàã, êëş÷ íå íàéäåí
                break; // êîíåö ñğàâíåíèÿ
            }
        }


        // ----------------
        if ( found != 0 ) // åñëè óêàçàííûé êëş÷ íàéäåí â ôàéëå
        {
            // åñëè íàéäåíî ñîâïàäåíèå íå öåëîãî êëş÷à ôàéëà, à åãî îêîí÷àíèÿ ñ óêàçàííûì êëş÷îì
            if ( curFilePos >= 0 )
            {
                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_KEY_STARTS : {}
                    default: continue;
                }
            }


            // ----------------
            // òåêóùàÿ ïîçèöèÿ â ôàéëå áóäåò íà 1 áîëüøå òåêóùåé ïîçèöèè INI_DELIMITER
            curFilePos = _ini_nDelimPos[nFilePointer][kPos] + 1;

            // èùåì ïîçèöèş íà÷àëà çíà÷åíèÿ, îíà áóäåò ïîìåùåíà â curFilePos
            for ( ; ; curFilePos++ )
            {
                if ( curFilePos >= _ini_nFileSize[nFilePointer] ) break;

                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_SPACE_CHAR :   continue; // åñëè ıòî ïğîáåëüíûé ñèìâîë, ïåğåéäåì ê ñëåä. ñèìâîëó
                    default :               break; // åñëè ıòî äğóãîé ñèìâîë
                }
            }
            // ----------------


            // ----------------
            new strValue[INI_FLOAT_SIZE]; // âğåìåííàÿ ñòğîêà äëÿ äğîáíîãî çíà÷åíèÿ

            // ñêîïèğóåì ïîñèìâîëüíî â strValue çíà÷åíèå êëş÷à èç ôàéëà
            // âîñïîëüçóåìñÿ ñîçäàííîé ïåğåìåííîé found êàê ïîçèöèåé â âîçâğàùàåìîì çíà÷åíèè
            for ( found = 0;  found < INI_INTEGER_SIZE;  found++, curFilePos++ )
            {
                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_NUMBER_ENDS : // åñëè ıòî ñèìâîë êîíöà ÷èñëåííîãî çíà÷åíèÿ
                    {
                        strValue[found] = 0; // çàïèøåì ñèìâîë êîíöà ñòğîêè
                        break; // êîíåö êîïèğîâàíèÿ
                    }
                    default :
                        // êîïèğóåì ñèìâîë èç ôàéëà â strValue
                        strValue[found] = _ini_szFileContent[nFilePointer]{curFilePos};
                }
            }

            strValue[INI_FLOAT_SIZE - 1] = 0; // íà âñÿêèé ñëó÷àé îáğåæåì ïğàâèëüíî ñòğîêó

            fReturnValue = floatstr(strValue); // çàïèøåì â fReturnValue äğîáíîå çíà÷åíèå êëş÷à

            return INI_OK;
            // ----------------
        }
        // ----------------
    }
    // ----------------


    // ----------------
    return INI_KEY_NOT_FOUND;
    // ----------------
}











/*
    Èçìåíÿåò/äîáàâëÿåò â îòêğûòûé INI ôàéë óêàçàííûé êëş÷ è åãî çíà÷åíèå.

    ÏÎÄĞÎÁÍÅÅ
        Ïàğñåğ èùåò â ÎÇÓ â êîíòåíòå ôàéëà óêàçàííûé êëş÷ è èçìåíÿåò åãî
        ñòğîêîâîå çíà÷åíèå íà szKeyValue.

        Ôîğìàòèğîâàíèå ôàéëà íå ìåíÿåòñÿ. Åñëè òîëüêî óêàçàííîå èìÿ êëş÷à
        íå áûëî íàéäåíî, òîãäà êëş÷ è çíà÷åíèå áóäóò äîáàâëåíû â êîíåö ôàéëà.

        Èìÿ êëş÷à/çíà÷åíèå â ôàéëå ìîæåò áûòü ëşáîé äëèíû, à òàêæå ìîæåò ñîäåğæàòü
        ëşáûå ñèìâîëû, êğîìå 2 ñèìâîëîâ êîíöà ñòğîêè \r è \n. Åñëè ıòè ñèìâîëû åñòü
        â èìåíè êëş÷à èëè â çíà÷åíèè, âû äîëæíû äîëæíû õîğîøî çíàòü è îñîçíàâàòü
        ïîñëåäñòâèÿ.

    ÏÀĞÀÌÅÒĞÛ:
        nFilePointer        ÈÄ_îòêğûòîãî_ôàéëà, ïîëó÷åííûé îò ini_openFile / ini_createFile
        szKeyName[]         èìÿ êëş÷à
        szKeyValue[]        ñòğîêà ñî çíà÷åíèåì êëş÷à

    ÂÅĞÍÅÒ:
        êîä îøèáêè < 0 èëè 0 ïğè óñïåõå
*/

stock ini_setString ( nFilePointer, szKeyName[], szKeyValue[] )
{
    // ----------------
    if // åñëè ÈÄ îòêğûòîãî ôàéëà óêàçàí íåâåğíî
    (
            nFilePointer < 0
        ||  nFilePointer >= INI_MAX_OPENED_FILES
        ||  _ini_nSlotUsed[nFilePointer] != 1
    )
        return INI_WRONG_SLOT; // âåğíóòü êîä: íåâåğíûé óêàçàòåëü íà îòêğûòûé ôàéë
    // ----------------


    // ----------------
    new nKeyLen = strlen(szKeyName); // óçíàåì äëèíó èìåíè óêàçàííîãî êëş÷à

    if ( nKeyLen <= 0 ) // åñëè óêàçàí ïóñòîé êëş÷
        return INI_KEY_NOT_FOUND;
    // ----------------


    // ----------------
    for // ïåğåáîğ è ñğàâíåíèå âñåõ êëş÷åé ôàéëà ñ óêàçàííûì êëş÷îì
    (
        new kPos = 0, curFilePos, found;
        kPos < _ini_nKeysCount[nFilePointer];
        kPos++
    )
    {
        // ----------------
        found = 0; // ôëàã, íàéäåíà ëè ïîçèöèÿ êîíöà êëş÷à = 0

        for // èùåì ïîçèöèş êîíöà êëş÷à
        (
            curFilePos = _ini_nDelimPos[nFilePointer][kPos] - 1; // òåêóù. ïîç. ôàéëà = ïîç. òåêóù. INI_DELIMITER - 1
            curFilePos >= 0; // ïğîäîëæàòü ïîêà ïîç. ôàéëà >= 0
            curFilePos-- // ïîñëå êàæäîãî ïîâòîğà òåêóù. ïîç. ôàéëà -= 1
        )
        {
            switch ( _ini_szFileContent[nFilePointer]{curFilePos} ) // óçíàåì ÷òî çà ñèìâîë â òåêóù. ïîç. ôàéëà
            {
                case INI_SPACE_CHAR :           continue; // åñëè ıòî ïğîáåëüíûé ñèìâîë, ïåğåéäåì ê ïğåä. ñèìâîëó ôàéëà
                case INI_STRING_DELIMITER :     break; // åñëè ıòî êîíåö ñòğîêè
                default : // åñëè ıòî äğóãîé ñèìâîë
                {
                    found = 1; // ïîçèöèÿ êîíöà êëş÷à íàéäåíà
                    break; // êîíåö öèêëà
                }
            }
        }

        // åñëè ïîçèöèÿ êîíöà êëş÷à íå íàéäåíà, ïåğåõîä ê ñëåä. ïîçèöèè INI_DELIMITER
        if ( found != 1 ) continue;
        // ----------------


        // ñğàâíèâàåì ïîñèìâîëüíî òåêóùèé êëş÷ ôàéëà è óêàçàííûé êëş÷
        for ( new curKeyPos = nKeyLen - 1;  curKeyPos >= 0;  curFilePos--, curKeyPos-- )
        {
            if
            (
                    curFilePos < 0 // åñëè ïîç ôàéëà ñòàëà < 0
                ||  _ini_szFileContent[nFilePointer]{curFilePos} != szKeyName[curKeyPos] // åñëè ñèìâîëû èç êëş÷åé íå ğàâíû
                ||  _ini_szFileContent[nFilePointer]{curFilePos} == INI_STRING_DELIMITER // åñëè ñèìâîë èç êëş÷à ıòî INI_STRING_DELIMITER
            )
            {
                found = 0; // ôëàã, êëş÷ íå íàéäåí
                break; // êîíåö ñğàâíåíèÿ
            }
        }


        if ( found != 0 ) // åñëè óêàçàííûé êëş÷ ÍÀÉÄÅÍ â ôàéëå
        {
            // åñëè íàéäåíî ñîâïàäåíèå íå öåëîãî êëş÷à ôàéëà, à åãî îêîí÷àíèÿ ñ óêàçàííûì êëş÷îì
            if ( curFilePos >= 0 )
            {
                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_KEY_STARTS : {}
                    default: continue;
                }
            }


            // ----------------
            // òåêóùàÿ ïîçèöèÿ â ôàéëå áóäåò íà 1 áîëüøå òåêóùåé ïîçèöèè INI_DELIMITER
            curFilePos = _ini_nDelimPos[nFilePointer][kPos] + 1;

            // èùåì ïîçèöèş íà÷àëà çíà÷åíèÿ, îíà áóäåò ïîìåùåíà â curFilePos
            for ( ; ; curFilePos++ )
            {
                if ( curFilePos >= _ini_nFileSize[nFilePointer] ) break;

                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_SPACE_CHAR :   continue; // åñëè ıòî ïğîáåëüíûé ñèìâîë, ïåğåéäåì ê ñëåä. ñèìâîëó
                    default :               break; // åñëè ıòî äğóãîé ñèìâîë
                }
            }
            // ----------------


            // ----------------
            new nValueLen = strlen(szKeyValue); // óçíàåì ğàçìåğ çíà÷åíèÿ óêàçàííîãî êëş÷à

            // åñëè áóäóùèé ğàçìåğ ôàéëà ïğåâûøàåò ëèìèò
            if ( ( curFilePos + nValueLen ) >= INI_MAX_FILE_SIZE )
                return INI_TOO_LARGE_FILE; // âåğíåì êîä îøèáêè î ïåğåïîëíåíèè ôàéëà
            // ----------------


            // ----------------
            new fileValueStartPos = curFilePos; // ñîõğàíèì ïîçèöèş íà÷àëà çíà÷åíèÿ

            // èùåì ïîçèöèş êîíöà çíà÷åíèÿ, îíà áóäåò ïîìåùåíà â curFilePos
            for ( ; ; curFilePos++ )
            {
                if ( curFilePos >= _ini_nFileSize[nFilePointer] ) break;

                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_STRING_ENDS :  break; // åñëè ıòî êîíåö ñòğîêè - ñòîï
                    default :               continue; // åñëè ıòî äğóãîé ñèìâîë, ïåğåéäåì ê ñëåä. ñèìâîëó
                }
            }

            // âû÷èñëèì ñìåùåíèå îñòàâøèõñÿ ïîçèöèé äëÿ INI_DELIMITER ïîñëå èçìåíåíèÿ çíà÷åíèÿ
            new filePosOffset = nValueLen - ( /*òåêóù äëèíà çíà÷åíèÿ*/ curFilePos - fileValueStartPos );
            // ----------------


            if ( _ini_nFileBusy[nFilePointer] != 0 ) // åñëè ïğÿìî ñåé÷àñ ôàéë èçìåíÿåòñÿ äğóãîé ôóíêöèåé
                return INI_WRITE_ERROR; // âåğíåì êîä îøèáêè ïğè çàïèñè â ôàéë


            // ----------------
            _ini_nFileBusy[nFilePointer] = 1; // ôëàã: ôàéë èçìåíÿåòñÿ = 1

            if ( filePosOffset != 0 ) // åñëè äëèíû ñòàğîãî è íîâîãî çíà÷åíèé ğàçíûå
            {
                if ( filePosOffset < 0 ) // åñëè äëèíà íîâîãî çíà÷åíèÿ ìåíüøå
                {
                    for // êîïèğóåì ñèìâîëû, ñòîÿùèå ïîñëå òåêóùåãî çíà÷åíèÿ íà èõ íîâûå ìåñòà,
                    ( // êîòîğûå òåïåğü áóäóò áëèæå ê íà÷àëó ôàéëà
                        ;
                        curFilePos < _ini_nFileSize[nFilePointer];
                        curFilePos++
                    )
                        _ini_szFileContent[nFilePointer]{ curFilePos + filePosOffset } =
                            _ini_szFileContent[nFilePointer]{curFilePos};
                }
                else if ( filePosOffset > 0 ) // åñëè äëèíà íîâîãî çíà÷åíèÿ áîëüøå
                {
                    new fileValueEndPos = curFilePos;

                    for // êîïèğóåì ñèìâîëû, ñòîÿùèå ïîñëå òåêóùåãî çíà÷åíèÿ íà èõ íîâûå ìåñòà,
                    ( // íà÷èíàÿ ñ êîíöà ôàéëà
                        curFilePos = _ini_nFileSize[nFilePointer] - 1;
                        curFilePos >= fileValueEndPos;
                        curFilePos--
                    )
                        _ini_szFileContent[nFilePointer]{ curFilePos + filePosOffset } =
                            _ini_szFileContent[nFilePointer]{curFilePos};
                }

                // èçìåíèì ïîçèöèè âñåõ INI_DELIMITER, êîòîğûå íàõîäèëèñü ïîñëå òåêóùåãî INI_DELIMITER
                for ( kPos++;  kPos < _ini_nKeysCount[nFilePointer];  kPos++ )
                    _ini_nDelimPos[nFilePointer][kPos] += filePosOffset;

                _ini_nFileSize[nFilePointer] += filePosOffset; // èçìåíèì ğàçìåğ ôàéëà
                _ini_szFileContent[nFilePointer]{ _ini_nFileSize[nFilePointer] } = 0; // êîíåö ñòğîêè äëÿ êîíöà ôàéëà
            }

            curFilePos = fileValueStartPos; // òåêóù ïîç ôàéëà = ïîç íà÷àëà ñòàğîãî çíà÷åíèÿ
            for // çàïèøåì íîâîå çíà÷åíèå ïîâåğõ ñòàğîãî
            (
                new valuePos = 0;
                valuePos < nValueLen;
                curFilePos++, valuePos++
            )
                _ini_szFileContent[nFilePointer]{curFilePos} = szKeyValue[valuePos];

            _ini_nFileChanged[nFilePointer]     = 1; // ôëàã: ôàéë èçìåíåí = 1
            _ini_nFileBusy[nFilePointer]        = 0; // ôëàã: ôàéë èçìåíÿåòñÿ = 0

            return INI_OK;
            // ----------------
        }
    }
    // ----------------


    //
    // åñëè óêàçàííûé êëş÷ ÍÅ ÍÀÉÄÅÍ â ôàéëå
    //

    // ----------------
    new nValueLen = strlen(szKeyValue); // óçíàåì ğàçìåğ çíà÷åíèÿ êëş÷à

    // åñëè áóäóùèé ğàçìåğ ôàéëà ïğåâûøàåò ëèìèò
    if ( ( _ini_nFileSize[nFilePointer] + 5 + nKeyLen + nValueLen ) >= INI_MAX_FILE_SIZE )
        return INI_TOO_LARGE_FILE; // âåğíåì êîä îøèáêè î ïåğåïîëíåíèè ôàéëà
    // ----------------


    // ----------------
    if ( _ini_nFileBusy[nFilePointer] != 0 ) // åñëè ïğÿìî ñåé÷àñ ôàéë èçìåíÿåòñÿ äğóãîé ôóíêöèåé
        return INI_WRITE_ERROR; // âåğíåì êîä îøèáêè ïğè çàïèñè â ôàéë

    _ini_nFileBusy[nFilePointer] = 1; // ôëàã: ôàéë èçìåíÿåòñÿ = 1
    // ----------------


    // ----------------
    if // åñëè ïîñëåäíèé ñèìâîë ôàéëà íå ÿâëÿåòñÿ ğàçäåëèòåëåì ñòğîê
    (
            _ini_nFileSize[nFilePointer] > 0
        &&  _ini_szFileContent[nFilePointer]{ _ini_nFileSize[nFilePointer] - 1 } != INI_STRING_DELIMITER
    )
    {
        // âñòàâèì â êîíåö ôàéëà ïåğåâîä êàğåòêè è ğàçäåëèòåëü ñòğîê
        _ini_szFileContent[nFilePointer]{ _ini_nFileSize[nFilePointer] }        = '\r';
        _ini_szFileContent[nFilePointer]{ _ini_nFileSize[nFilePointer] + 1 }    = INI_STRING_DELIMITER;

        _ini_nFileSize[nFilePointer] += 2; // óâåëè÷èì ğàçìåğ ôàéëà íà 2
    }

    new curFilePos, strPos; // áóäóò âğåìåííûì õğàíèëèùàìè ïîçèöèé â ôàéëå/êëş÷å/çíà÷åíèè

    for // äîáàâèì ïîñèìâîëüíî èìÿ êëş÷à â ôàéë
    (
        curFilePos = _ini_nFileSize[nFilePointer], strPos = 0;
        strPos < nKeyLen;
        curFilePos++, strPos++
    )
        _ini_szFileContent[nFilePointer]{curFilePos} = szKeyName[strPos];

    _ini_szFileContent[nFilePointer]{ curFilePos++ }                = ' '; // ïîñëå êëş÷à äîáàâèì ïğîáåë

    _ini_nDelimPos[nFilePointer][ _ini_nKeysCount[nFilePointer] ]   = curFilePos; // äîáàâèì íîâóş ïîçèöèş INI_DELIMITER
    _ini_nKeysCount[nFilePointer]++; // êîë-âî êëş÷åé â ôàéëå +1

    _ini_szFileContent[nFilePointer]{ curFilePos++ }                = INI_DELIMITER; // ïîñëå ïğîáåëà âûøå äîáàâèì INI_DELIMITER
    _ini_szFileContent[nFilePointer]{ curFilePos++ }                = ' '; // ïîñëå INI_DELIMITER äîáàâèì ïğîáåë

    // äîáàâèì ïîñèìâîëüíî çíà÷åíèå êëş÷à â ôàéë
    for ( strPos = 0;  strPos < nValueLen;  curFilePos++, strPos++ )
        _ini_szFileContent[nFilePointer]{curFilePos} = szKeyValue[strPos];

    _ini_szFileContent[nFilePointer]{curFilePos}    = 0; // äîáàâèì ñèìâîë êîíöà ñòğîêè
    _ini_nFileSize[nFilePointer]                    = curFilePos; // èçìåíèì ğàçìåğ ôàéëà
    _ini_nFileChanged[nFilePointer]                 = 1; // ôëàã: ôàéë èçìåíåí = 1
    _ini_nFileBusy[nFilePointer]                    = 0; // ôëàã: ôàéë èçìåíÿåòñÿ = 0

    return INI_OK;
    // ----------------
}




/*
    Èçìåíÿåò/äîáàâëÿåò â îòêğûòûé INI ôàéë óêàçàííûé êëş÷ è åãî öåëî÷èñëåííîå çíà÷åíèå.

    ÏÎÄĞÎÁÍÅÅ
        Ïàğñåğ èùåò â ÎÇÓ â êîíòåíòå ôàéëà óêàçàííûé êëş÷ è èçìåíÿåò åãî
        öåëî÷èñëåííîå çíà÷åíèå íà nKeyValue.

        Ôîğìàòèğîâàíèå ôàéëà íå ìåíÿåòñÿ, åñëè òîëüêî óêàçàííîå èìÿ êëş÷à
        íå áûëî íàéäåíî, òîãäà êëş÷ è çíà÷åíèå áóäóò äîáàâëåíû â êîíåö ôàéëà.

        Èìÿ êëş÷à â ôàéëå ìîæåò áûòü ëşáîé äëèíû, à òàêæå ìîæåò ñîäåğæàòü
        ëşáûå ñèìâîëû, êğîìå 2 ñèìâîëîâ êîíöà ñòğîêè \r è \n. Åñëè âû â êà÷åñòâå
        çíà÷åíèÿ óêàçûâàåòå ÷èñëî áîëüøå, ÷åì cellmax èëè ìåíüøå, ÷åì cellmin,
        âû äîëæíû äîëæíû õîğîøî çíàòü è îñîçíàâàòü ïîñëåäñòâèÿ.

    ÏÀĞÀÌÅÒĞÛ:
        nFilePointer        ÈÄ_îòêğûòîãî_ôàéëà, ïîëó÷åííûé îò ini_openFile / ini_createFile
        szKeyName[]         èìÿ êëş÷à
        nKeyValue           öåëî÷èñëåííîå çíà÷åíèå êëş÷à

    ÂÅĞÍÅÒ:
        êîä îøèáêè < 0 èëè 0 ïğè óñïåõå
*/

stock ini_setInteger ( nFilePointer, szKeyName[], nKeyValue )
{
    // ----------------
    if // åñëè ÈÄ îòêğûòîãî ôàéëà óêàçàí íåâåğíî
    (
            nFilePointer < 0
        ||  nFilePointer >= INI_MAX_OPENED_FILES
        ||  _ini_nSlotUsed[nFilePointer] != 1
    )
        return INI_WRONG_SLOT; // âåğíóòü êîä: íåâåğíûé óêàçàòåëü íà îòêğûòûé ôàéë
    // ----------------


    // ----------------
    new nKeyLen = strlen(szKeyName); // óçíàåì äëèíó èìåíè óêàçàííîãî êëş÷à

    if ( nKeyLen <= 0 ) // åñëè óêàçàí ïóñòîé êëş÷
        return INI_KEY_NOT_FOUND;
    // ----------------


    // ----------------
    for // ïåğåáîğ è ñğàâíåíèå âñåõ êëş÷åé ôàéëà ñ óêàçàííûì êëş÷îì
    (
        new kPos = 0, curFilePos, found;
        kPos < _ini_nKeysCount[nFilePointer];
        kPos++
    )
    {
        // ----------------
        found = 0; // ôëàã, íàéäåíà ëè ïîçèöèÿ êîíöà êëş÷à = 0

        for // èùåì ïîçèöèş êîíöà êëş÷à
        (
            curFilePos = _ini_nDelimPos[nFilePointer][kPos] - 1; // òåêóù. ïîç. ôàéëà = ïîç. òåêóù. INI_DELIMITER - 1
            curFilePos >= 0; // ïğîäîëæàòü ïîêà ïîç. ôàéëà >= 0
            curFilePos-- // ïîñëå êàæäîãî ïîâòîğà òåêóù. ïîç. ôàéëà -= 1
        )
        {
            switch ( _ini_szFileContent[nFilePointer]{curFilePos} ) // óçíàåì ÷òî çà ñèìâîë â òåêóù. ïîç. ôàéëà
            {
                case INI_SPACE_CHAR :           continue; // åñëè ıòî ïğîáåëüíûé ñèìâîë, ïåğåéäåì ê ïğåä. ñèìâîëó ôàéëà
                case INI_STRING_DELIMITER :     break; // åñëè ıòî êîíåö ñòğîêè
                default : // åñëè ıòî äğóãîé ñèìâîë
                {
                    found = 1; // ïîçèöèÿ êîíöà êëş÷à íàéäåíà
                    break; // êîíåö öèêëà
                }
            }
        }

        // åñëè ïîçèöèÿ êîíöà êëş÷à íå íàéäåíà, ïåğåõîä ê ñëåä. ïîçèöèè INI_DELIMITER
        if ( found != 1 ) continue;
        // ----------------


        // ñğàâíèâàåì ïîñèìâîëüíî òåêóùèé êëş÷ ôàéëà è óêàçàííûé êëş÷
        for ( new curKeyPos = nKeyLen - 1;  curKeyPos >= 0;  curFilePos--, curKeyPos-- )
        {
            if
            (
                    curFilePos < 0 // åñëè ïîç ôàéëà ñòàëà < 0
                ||  _ini_szFileContent[nFilePointer]{curFilePos} != szKeyName[curKeyPos] // åñëè ñèìâîëû èç êëş÷åé íå ğàâíû
                ||  _ini_szFileContent[nFilePointer]{curFilePos} == INI_STRING_DELIMITER // åñëè ñèìâîë èç êëş÷à ıòî INI_STRING_DELIMITER
            )
            {
                found = 0; // ôëàã, êëş÷ íå íàéäåí
                break; // êîíåö ñğàâíåíèÿ
            }
        }


        if ( found != 0 ) // åñëè óêàçàííûé êëş÷ ÍÀÉÄÅÍ â ôàéëå
        {
            // åñëè íàéäåíî ñîâïàäåíèå íå öåëîãî êëş÷à ôàéëà, à åãî îêîí÷àíèÿ ñ óêàçàííûì êëş÷îì
            if ( curFilePos >= 0 )
            {
                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_KEY_STARTS : {}
                    default: continue;
                }
            }


            // ----------------
            // òåêóùàÿ ïîçèöèÿ â ôàéëå áóäåò íà 1 áîëüøå òåêóùåé ïîçèöèè INI_DELIMITER
            curFilePos = _ini_nDelimPos[nFilePointer][kPos] + 1;

            // èùåì ïîçèöèş íà÷àëà çíà÷åíèÿ, îíà áóäåò ïîìåùåíà â curFilePos
            for ( ; ; curFilePos++ )
            {
                if ( curFilePos >= _ini_nFileSize[nFilePointer] ) break;

                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_SPACE_CHAR :   continue; // åñëè ıòî ïğîáåëüíûé ñèìâîë, ïåğåéäåì ê ñëåä. ñèìâîëó
                    default :               break; // åñëè ıòî äğóãîé ñèìâîë
                }
            }
            // ----------------


            // ----------------
            new szKeyValue[INI_INTEGER_SIZE]; // ñîçäàäèì ñòğîêó äëÿ õğàíåíèÿ öåëî÷èñëåííîãî çíà÷åíèÿ
            format( szKeyValue, INI_INTEGER_SIZE, "%i", nKeyValue );
            // ----------------


            // ----------------
            new nValueLen = strlen(szKeyValue); // óçíàåì ğàçìåğ çíà÷åíèÿ óêàçàííîãî êëş÷à

            // åñëè áóäóùèé ğàçìåğ ôàéëà ïğåâûøàåò ëèìèò
            if ( ( curFilePos + nValueLen ) >= INI_MAX_FILE_SIZE )
                return INI_TOO_LARGE_FILE; // âåğíåì êîä îøèáêè î ïåğåïîëíåíèè ôàéëà
            // ----------------


            // ----------------
            new fileValueStartPos = curFilePos; // ñîõğàíèì ïîçèöèş íà÷àëà çíà÷åíèÿ

            // èùåì ïîçèöèş êîíöà çíà÷åíèÿ, îíà áóäåò ïîìåùåíà â curFilePos
            for ( ; ; curFilePos++ )
            {
                if ( curFilePos >= _ini_nFileSize[nFilePointer] ) break;

                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_NUMBER_ENDS :  break; // åñëè ıòî êîíåö ÷èñëà - ñòîï
                    default :               continue; // åñëè ıòî äğóãîé ñèìâîë, ïåğåéäåì ê ñëåä. ñèìâîëó
                }
            }

            // âû÷èñëèì ñìåùåíèå îñòàâøèõñÿ ïîçèöèé äëÿ INI_DELIMITER ïîñëå èçìåíåíèÿ çíà÷åíèÿ
            new filePosOffset = nValueLen - ( /*òåêóù äëèíà çíà÷åíèÿ*/ curFilePos - fileValueStartPos );
            // ----------------


            if ( _ini_nFileBusy[nFilePointer] != 0 ) // åñëè ïğÿìî ñåé÷àñ ôàéë èçìåíÿåòñÿ äğóãîé ôóíêöèåé
                return INI_WRITE_ERROR; // âåğíåì êîä îøèáêè ïğè çàïèñè â ôàéë


            // ----------------
            _ini_nFileBusy[nFilePointer] = 1; // ôëàã: ôàéë èçìåíÿåòñÿ = 1

            if ( filePosOffset != 0 ) // åñëè äëèíû ñòàğîãî è íîâîãî çíà÷åíèé ğàçíûå
            {
                if ( filePosOffset < 0 ) // åñëè äëèíà íîâîãî çíà÷åíèÿ ìåíüøå
                {
                    for // êîïèğóåì ñèìâîëû, ñòîÿùèå ïîñëå òåêóùåãî çíà÷åíèÿ íà èõ íîâûå ìåñòà,
                    ( // êîòîğûå òåïåğü áóäóò áëèæå ê íà÷àëó ôàéëà
                        ;
                        curFilePos < _ini_nFileSize[nFilePointer];
                        curFilePos++
                    )
                        _ini_szFileContent[nFilePointer]{ curFilePos + filePosOffset } =
                            _ini_szFileContent[nFilePointer]{curFilePos};
                }
                else if ( filePosOffset > 0 ) // åñëè äëèíà íîâîãî çíà÷åíèÿ áîëüøå
                {
                    new fileValueEndPos = curFilePos;

                    for // êîïèğóåì ñèìâîëû, ñòîÿùèå ïîñëå òåêóùåãî çíà÷åíèÿ íà èõ íîâûå ìåñòà,
                    ( // íà÷èíàÿ ñ êîíöà ôàéëà
                        curFilePos = _ini_nFileSize[nFilePointer] - 1;
                        curFilePos >= fileValueEndPos;
                        curFilePos--
                    )
                        _ini_szFileContent[nFilePointer]{ curFilePos + filePosOffset } =
                            _ini_szFileContent[nFilePointer]{curFilePos};
                }

                // èçìåíèì ïîçèöèè âñåõ INI_DELIMITER, êîòîğûå íàõîäèëèñü ïîñëå òåêóùåãî INI_DELIMITER
                for ( kPos++;  kPos < _ini_nKeysCount[nFilePointer];  kPos++ )
                    _ini_nDelimPos[nFilePointer][kPos] += filePosOffset;

                _ini_nFileSize[nFilePointer] += filePosOffset; // èçìåíèì ğàçìåğ ôàéëà
                _ini_szFileContent[nFilePointer]{ _ini_nFileSize[nFilePointer] } = 0; // êîíåö ñòğîêè äëÿ êîíöà ôàéëà
            }

            curFilePos = fileValueStartPos; // òåêóù ïîç ôàéëà = ïîç íà÷àëà ñòàğîãî çíà÷åíèÿ
            for // çàïèøåì íîâîå çíà÷åíèå ïîâåğõ ñòàğîãî
            (
                new valuePos = 0;
                valuePos < nValueLen;
                curFilePos++, valuePos++
            )
                _ini_szFileContent[nFilePointer]{curFilePos} = szKeyValue[valuePos];

            _ini_nFileChanged[nFilePointer]     = 1; // ôëàã: ôàéë èçìåíåí = 1
            _ini_nFileBusy[nFilePointer]        = 0; // ôëàã: ôàéë èçìåíÿåòñÿ = 0

            return INI_OK;
            // ----------------
        }
    }
    // ----------------


    //
    // åñëè óêàçàííûé êëş÷ ÍÅ ÍÀÉÄÅÍ â ôàéëå
    //

    // ----------------
    new szKeyValue[INI_INTEGER_SIZE]; // ñîçäàäèì ñòğîêó äëÿ õğàíåíèÿ öåëî÷èñëåííîãî çíà÷åíèÿ
    format( szKeyValue, INI_INTEGER_SIZE, "%i", nKeyValue );
    // ----------------


    // ----------------
    new nValueLen = strlen(szKeyValue); // óçíàåì ğàçìåğ çíà÷åíèÿ êëş÷à

    // åñëè áóäóùèé ğàçìåğ ôàéëà ïğåâûøàåò ëèìèò
    if ( ( _ini_nFileSize[nFilePointer] + 5 + nKeyLen + nValueLen ) >= INI_MAX_FILE_SIZE )
        return INI_TOO_LARGE_FILE; // âåğíåì êîä îøèáêè î ïåğåïîëíåíèè ôàéëà
    // ----------------


    // ----------------
    if ( _ini_nFileBusy[nFilePointer] != 0 ) // åñëè ïğÿìî ñåé÷àñ ôàéë èçìåíÿåòñÿ äğóãîé ôóíêöèåé
        return INI_WRITE_ERROR; // âåğíåì êîä îøèáêè ïğè çàïèñè â ôàéë

    _ini_nFileBusy[nFilePointer] = 1; // ôëàã: ôàéë èçìåíÿåòñÿ = 1
    // ----------------


    // ----------------
    if // åñëè ïîñëåäíèé ñèìâîë ôàéëà íå ÿâëÿåòñÿ ğàçäåëèòåëåì ñòğîê
    (
            _ini_nFileSize[nFilePointer] > 0
        &&  _ini_szFileContent[nFilePointer]{ _ini_nFileSize[nFilePointer] - 1 } != INI_STRING_DELIMITER
    )
    {
        // âñòàâèì â êîíåö ôàéëà ïåğåâîä êàğåòêè è ğàçäåëèòåëü ñòğîê
        _ini_szFileContent[nFilePointer]{ _ini_nFileSize[nFilePointer] }        = '\r';
        _ini_szFileContent[nFilePointer]{ _ini_nFileSize[nFilePointer] + 1 }    = INI_STRING_DELIMITER;

        _ini_nFileSize[nFilePointer] += 2; // óâåëè÷èì ğàçìåğ ôàéëà íà 2
    }

    new curFilePos, strPos; // áóäóò âğåìåííûì õğàíèëèùàìè ïîçèöèé â ôàéëå/êëş÷å/çíà÷åíèè

    for // äîáàâèì ïîñèìâîëüíî èìÿ êëş÷à â ôàéë
    (
        curFilePos = _ini_nFileSize[nFilePointer], strPos = 0;
        strPos < nKeyLen;
        curFilePos++, strPos++
    )
        _ini_szFileContent[nFilePointer]{curFilePos} = szKeyName[strPos];

    _ini_szFileContent[nFilePointer]{ curFilePos++ }                = ' '; // ïîñëå êëş÷à äîáàâèì ïğîáåë

    _ini_nDelimPos[nFilePointer][ _ini_nKeysCount[nFilePointer] ]   = curFilePos; // äîáàâèì íîâóş ïîçèöèş INI_DELIMITER
    _ini_nKeysCount[nFilePointer]++; // êîë-âî êëş÷åé â ôàéëå +1

    _ini_szFileContent[nFilePointer]{ curFilePos++ }                = INI_DELIMITER; // ïîñëå ïğîáåëà âûøå äîáàâèì INI_DELIMITER
    _ini_szFileContent[nFilePointer]{ curFilePos++ }                = ' '; // ïîñëå INI_DELIMITER äîáàâèì ïğîáåë

    // äîáàâèì ïîñèìâîëüíî çíà÷åíèå êëş÷à â ôàéë
    for ( strPos = 0;  strPos < nValueLen;  curFilePos++, strPos++ )
        _ini_szFileContent[nFilePointer]{curFilePos} = szKeyValue[strPos];

    _ini_szFileContent[nFilePointer]{curFilePos}    = 0; // äîáàâèì ñèìâîë êîíöà ñòğîêè
    _ini_nFileSize[nFilePointer]                    = curFilePos; // èçìåíèì ğàçìåğ ôàéëà
    _ini_nFileChanged[nFilePointer]                 = 1; // ôëàã: ôàéë èçìåíåí = 1
    _ini_nFileBusy[nFilePointer]                    = 0; // ôëàã: ôàéë èçìåíÿåòñÿ = 0

    return INI_OK;
    // ----------------
}




/*
    Èçìåíÿåò/äîáàâëÿåò â îòêğûòûé INI ôàéë óêàçàííûé êëş÷ è åãî äğîáíîå ÷èñëåííîå çíà÷åíèå.

    ÏÎÄĞÎÁÍÅÅ
        Ïàğñåğ èùåò â ÎÇÓ â êîíòåíòå ôàéëà óêàçàííûé êëş÷ è èçìåíÿåò åãî
        äğîáíîå çíà÷åíèå íà fKeyValue.

        Ôîğìàòèğîâàíèå ôàéëà íå ìåíÿåòñÿ, åñëè òîëüêî óêàçàííîå èìÿ êëş÷à
        íå áûëî íàéäåíî, òîãäà êëş÷ è çíà÷åíèå áóäóò äîáàâëåíû â êîíåö ôàéëà.

        Èìÿ êëş÷à â ôàéëå ìîæåò áûòü ëşáîé äëèíû, à òàêæå ìîæåò ñîäåğæàòü
        ëşáûå ñèìâîëû, êğîìå 2 ñèìâîëîâ êîíöà ñòğîêè \r è \n. Åñëè âû â êà÷åñòâå
        çíà÷åíèÿ óêàçûâàåòå î÷åíü áîëüøîå/ìàëåíüêîå äğîáíîå ÷èñëî, âû äîëæíû
        õîğîøî çíàòü è îñîçíàâàòü ïîñëåäñòâèÿ.

    ÏÀĞÀÌÅÒĞÛ:
        nFilePointer        ÈÄ_îòêğûòîãî_ôàéëà, ïîëó÷åííûé îò ini_openFile / ini_createFile
        szKeyName[]         èìÿ êëş÷à
        fKeyValue           äğîáíîå ÷èñëåííîå çíà÷åíèå êëş÷à

    ÂÅĞÍÅÒ:
        êîä îøèáêè < 0 èëè 0 ïğè óñïåõå
*/

stock ini_setFloat ( nFilePointer, szKeyName[], Float: fKeyValue )
{
    // ----------------
    if // åñëè ÈÄ îòêğûòîãî ôàéëà óêàçàí íåâåğíî
    (
            nFilePointer < 0
        ||  nFilePointer >= INI_MAX_OPENED_FILES
        ||  _ini_nSlotUsed[nFilePointer] != 1
    )
        return INI_WRONG_SLOT; // âåğíóòü êîä: íåâåğíûé óêàçàòåëü íà îòêğûòûé ôàéë
    // ----------------


    // ----------------
    new nKeyLen = strlen(szKeyName); // óçíàåì äëèíó èìåíè óêàçàííîãî êëş÷à

    if ( nKeyLen <= 0 ) // åñëè óêàçàí ïóñòîé êëş÷
        return INI_KEY_NOT_FOUND;
    // ----------------


    // ----------------
    for // ïåğåáîğ è ñğàâíåíèå âñåõ êëş÷åé ôàéëà ñ óêàçàííûì êëş÷îì
    (
        new kPos = 0, curFilePos, found;
        kPos < _ini_nKeysCount[nFilePointer];
        kPos++
    )
    {
        // ----------------
        found = 0; // ôëàã, íàéäåíà ëè ïîçèöèÿ êîíöà êëş÷à = 0

        for // èùåì ïîçèöèş êîíöà êëş÷à
        (
            curFilePos = _ini_nDelimPos[nFilePointer][kPos] - 1; // òåêóù. ïîç. ôàéëà = ïîç. òåêóù. INI_DELIMITER - 1
            curFilePos >= 0; // ïğîäîëæàòü ïîêà ïîç. ôàéëà >= 0
            curFilePos-- // ïîñëå êàæäîãî ïîâòîğà òåêóù. ïîç. ôàéëà -= 1
        )
        {
            switch ( _ini_szFileContent[nFilePointer]{curFilePos} ) // óçíàåì ÷òî çà ñèìâîë â òåêóù. ïîç. ôàéëà
            {
                case INI_SPACE_CHAR :           continue; // åñëè ıòî ïğîáåëüíûé ñèìâîë, ïåğåéäåì ê ïğåä. ñèìâîëó ôàéëà
                case INI_STRING_DELIMITER :     break; // åñëè ıòî êîíåö ñòğîêè
                default : // åñëè ıòî äğóãîé ñèìâîë
                {
                    found = 1; // ïîçèöèÿ êîíöà êëş÷à íàéäåíà
                    break; // êîíåö öèêëà
                }
            }
        }

        // åñëè ïîçèöèÿ êîíöà êëş÷à íå íàéäåíà, ïåğåõîä ê ñëåä. ïîçèöèè INI_DELIMITER
        if ( found != 1 ) continue;
        // ----------------


        // ñğàâíèâàåì ïîñèìâîëüíî òåêóùèé êëş÷ ôàéëà è óêàçàííûé êëş÷
        for ( new curKeyPos = nKeyLen - 1;  curKeyPos >= 0;  curFilePos--, curKeyPos-- )
        {
            if
            (
                    curFilePos < 0 // åñëè ïîç ôàéëà ñòàëà < 0
                ||  _ini_szFileContent[nFilePointer]{curFilePos} != szKeyName[curKeyPos] // åñëè ñèìâîëû èç êëş÷åé íå ğàâíû
                ||  _ini_szFileContent[nFilePointer]{curFilePos} == INI_STRING_DELIMITER // åñëè ñèìâîë èç êëş÷à ıòî INI_STRING_DELIMITER
            )
            {
                found = 0; // ôëàã, êëş÷ íå íàéäåí
                break; // êîíåö ñğàâíåíèÿ
            }
        }


        if ( found != 0 ) // åñëè óêàçàííûé êëş÷ ÍÀÉÄÅÍ â ôàéëå
        {
            // åñëè íàéäåíî ñîâïàäåíèå íå öåëîãî êëş÷à ôàéëà, à åãî îêîí÷àíèÿ ñ óêàçàííûì êëş÷îì
            if ( curFilePos >= 0 )
            {
                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_KEY_STARTS : {}
                    default: continue;
                }
            }


            // ----------------
            // òåêóùàÿ ïîçèöèÿ â ôàéëå áóäåò íà 1 áîëüøå òåêóùåé ïîçèöèè INI_DELIMITER
            curFilePos = _ini_nDelimPos[nFilePointer][kPos] + 1;

            // èùåì ïîçèöèş íà÷àëà çíà÷åíèÿ, îíà áóäåò ïîìåùåíà â curFilePos
            for ( ; ; curFilePos++ )
            {
                if ( curFilePos >= _ini_nFileSize[nFilePointer] ) break;

                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_SPACE_CHAR :   continue; // åñëè ıòî ïğîáåëüíûé ñèìâîë, ïåğåéäåì ê ñëåä. ñèìâîëó
                    default :               break; // åñëè ıòî äğóãîé ñèìâîë
                }
            }
            // ----------------


            // ----------------
            new szKeyValue[INI_FLOAT_SIZE]; // ñîçäàäèì ñòğîêó äëÿ õğàíåíèÿ äğîáíîãî çíà÷åíèÿ
            format( szKeyValue, INI_FLOAT_SIZE, "%f", fKeyValue ); // ïîìåñòèì ñòğîêîâîå çíà÷åíèå fKeyValue â szKeyValue
            // ----------------


            // ----------------
            new nValueLen = strlen(szKeyValue); // óçíàåì ğàçìåğ çíà÷åíèÿ óêàçàííîãî êëş÷à

            // åñëè áóäóùèé ğàçìåğ ôàéëà ïğåâûøàåò ëèìèò
            if ( ( curFilePos + nValueLen ) >= INI_MAX_FILE_SIZE )
                return INI_TOO_LARGE_FILE; // âåğíåì êîä îøèáêè î ïåğåïîëíåíèè ôàéëà
            // ----------------


            // ----------------
            new fileValueStartPos = curFilePos; // ñîõğàíèì ïîçèöèş íà÷àëà çíà÷åíèÿ

            // èùåì ïîçèöèş êîíöà çíà÷åíèÿ, îíà áóäåò ïîìåùåíà â curFilePos
            for ( ; ; curFilePos++ )
            {
                if ( curFilePos >= _ini_nFileSize[nFilePointer] ) break;

                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_NUMBER_ENDS :  break; // åñëè ıòî êîíåö ÷èñëà - ñòîï
                    default :               continue; // åñëè ıòî äğóãîé ñèìâîë, ïåğåéäåì ê ñëåä. ñèìâîëó
                }
            }

            // âû÷èñëèì ñìåùåíèå îñòàâøèõñÿ ïîçèöèé äëÿ INI_DELIMITER ïîñëå èçìåíåíèÿ çíà÷åíèÿ
            new filePosOffset = nValueLen - ( /*òåêóù äëèíà çíà÷åíèÿ*/ curFilePos - fileValueStartPos );
            // ----------------


            if ( _ini_nFileBusy[nFilePointer] != 0 ) // åñëè ïğÿìî ñåé÷àñ ôàéë èçìåíÿåòñÿ äğóãîé ôóíêöèåé
                return INI_WRITE_ERROR; // âåğíåì êîä îøèáêè ïğè çàïèñè â ôàéë


            // ----------------
            _ini_nFileBusy[nFilePointer] = 1; // ôëàã: ôàéë èçìåíÿåòñÿ = 1

            if ( filePosOffset != 0 ) // åñëè äëèíû ñòàğîãî è íîâîãî çíà÷åíèé ğàçíûå
            {
                if ( filePosOffset < 0 ) // åñëè äëèíà íîâîãî çíà÷åíèÿ ìåíüøå
                {
                    for // êîïèğóåì ñèìâîëû, ñòîÿùèå ïîñëå òåêóùåãî çíà÷åíèÿ íà èõ íîâûå ìåñòà,
                    ( // êîòîğûå òåïåğü áóäóò áëèæå ê íà÷àëó ôàéëà
                        ;
                        curFilePos < _ini_nFileSize[nFilePointer];
                        curFilePos++
                    )
                        _ini_szFileContent[nFilePointer]{ curFilePos + filePosOffset } =
                            _ini_szFileContent[nFilePointer]{curFilePos};
                }
                else if ( filePosOffset > 0 ) // åñëè äëèíà íîâîãî çíà÷åíèÿ áîëüøå
                {
                    new fileValueEndPos = curFilePos;

                    for // êîïèğóåì ñèìâîëû, ñòîÿùèå ïîñëå òåêóùåãî çíà÷åíèÿ íà èõ íîâûå ìåñòà,
                    ( // íà÷èíàÿ ñ êîíöà ôàéëà
                        curFilePos = _ini_nFileSize[nFilePointer] - 1;
                        curFilePos >= fileValueEndPos;
                        curFilePos--
                    )
                        _ini_szFileContent[nFilePointer]{ curFilePos + filePosOffset } =
                            _ini_szFileContent[nFilePointer]{curFilePos};
                }

                // èçìåíèì ïîçèöèè âñåõ INI_DELIMITER, êîòîğûå íàõîäèëèñü ïîñëå òåêóùåãî INI_DELIMITER
                for ( kPos++;  kPos < _ini_nKeysCount[nFilePointer];  kPos++ )
                    _ini_nDelimPos[nFilePointer][kPos] += filePosOffset;

                _ini_nFileSize[nFilePointer] += filePosOffset; // èçìåíèì ğàçìåğ ôàéëà
                _ini_szFileContent[nFilePointer]{ _ini_nFileSize[nFilePointer] } = 0; // êîíåö ñòğîêè äëÿ êîíöà ôàéëà
            }

            curFilePos = fileValueStartPos; // òåêóù ïîç ôàéëà = ïîç íà÷àëà ñòàğîãî çíà÷åíèÿ
            for // çàïèøåì íîâîå çíà÷åíèå ïîâåğõ ñòàğîãî
            (
                new valuePos = 0;
                valuePos < nValueLen;
                curFilePos++, valuePos++
            )
                _ini_szFileContent[nFilePointer]{curFilePos} = szKeyValue[valuePos];

            _ini_nFileChanged[nFilePointer]     = 1; // ôëàã: ôàéë èçìåíåí = 1
            _ini_nFileBusy[nFilePointer]        = 0; // ôëàã: ôàéë èçìåíÿåòñÿ = 0

            return INI_OK;
            // ----------------
        }
    }
    // ----------------


    //
    // åñëè óêàçàííûé êëş÷ ÍÅ ÍÀÉÄÅÍ â ôàéëå
    //

    // ----------------
    new szKeyValue[INI_FLOAT_SIZE]; // ñîçäàäèì ñòğîêó äëÿ õğàíåíèÿ äğîáíîãî çíà÷åíèÿ
    format( szKeyValue, INI_FLOAT_SIZE, "%f", fKeyValue ); // ïîìåñòèì ñòğîêîâîå çíà÷åíèå fKeyValue â szKeyValue
    // ----------------


    // ----------------
    new nValueLen = strlen(szKeyValue); // óçíàåì ğàçìåğ çíà÷åíèÿ êëş÷à

    // åñëè áóäóùèé ğàçìåğ ôàéëà ïğåâûøàåò ëèìèò
    if ( ( _ini_nFileSize[nFilePointer] + 5 + nKeyLen + nValueLen ) >= INI_MAX_FILE_SIZE )
        return INI_TOO_LARGE_FILE; // âåğíåì êîä îøèáêè î ïåğåïîëíåíèè ôàéëà
    // ----------------


    // ----------------
    if ( _ini_nFileBusy[nFilePointer] != 0 ) // åñëè ïğÿìî ñåé÷àñ ôàéë èçìåíÿåòñÿ äğóãîé ôóíêöèåé
        return INI_WRITE_ERROR; // âåğíåì êîä îøèáêè ïğè çàïèñè â ôàéë

    _ini_nFileBusy[nFilePointer] = 1; // ôëàã: ôàéë èçìåíÿåòñÿ = 1
    // ----------------


    // ----------------
    if // åñëè ïîñëåäíèé ñèìâîë ôàéëà íå ÿâëÿåòñÿ ğàçäåëèòåëåì ñòğîê
    (
            _ini_nFileSize[nFilePointer] > 0
        &&  _ini_szFileContent[nFilePointer]{ _ini_nFileSize[nFilePointer] - 1 } != INI_STRING_DELIMITER
    )
    {
        // âñòàâèì â êîíåö ôàéëà ïåğåâîä êàğåòêè è ğàçäåëèòåëü ñòğîê
        _ini_szFileContent[nFilePointer]{ _ini_nFileSize[nFilePointer] }        = '\r';
        _ini_szFileContent[nFilePointer]{ _ini_nFileSize[nFilePointer] + 1 }    = INI_STRING_DELIMITER;

        _ini_nFileSize[nFilePointer] += 2; // óâåëè÷èì ğàçìåğ ôàéëà íà 2
    }

    new curFilePos, strPos; // áóäóò âğåìåííûì õğàíèëèùàìè ïîçèöèé â ôàéëå/êëş÷å/çíà÷åíèè

    for // äîáàâèì ïîñèìâîëüíî èìÿ êëş÷à â ôàéë
    (
        curFilePos = _ini_nFileSize[nFilePointer], strPos = 0;
        strPos < nKeyLen;
        curFilePos++, strPos++
    )
        _ini_szFileContent[nFilePointer]{curFilePos} = szKeyName[strPos];

    _ini_szFileContent[nFilePointer]{ curFilePos++ }                = ' '; // ïîñëå êëş÷à äîáàâèì ïğîáåë

    _ini_nDelimPos[nFilePointer][ _ini_nKeysCount[nFilePointer] ]   = curFilePos; // äîáàâèì íîâóş ïîçèöèş INI_DELIMITER
    _ini_nKeysCount[nFilePointer]++; // êîë-âî êëş÷åé â ôàéëå +1

    _ini_szFileContent[nFilePointer]{ curFilePos++ }                = INI_DELIMITER; // ïîñëå ïğîáåëà âûøå äîáàâèì INI_DELIMITER
    _ini_szFileContent[nFilePointer]{ curFilePos++ }                = ' '; // ïîñëå INI_DELIMITER äîáàâèì ïğîáåë

    // äîáàâèì ïîñèìâîëüíî çíà÷åíèå êëş÷à â ôàéë
    for ( strPos = 0;  strPos < nValueLen;  curFilePos++, strPos++ )
        _ini_szFileContent[nFilePointer]{curFilePos} = szKeyValue[strPos];

    _ini_szFileContent[nFilePointer]{curFilePos}    = 0; // äîáàâèì ñèìâîë êîíöà ñòğîêè
    _ini_nFileSize[nFilePointer]                    = curFilePos; // èçìåíèì ğàçìåğ ôàéëà
    _ini_nFileChanged[nFilePointer]                 = 1; // ôëàã: ôàéë èçìåíåí = 1
    _ini_nFileBusy[nFilePointer]                    = 0; // ôëàã: ôàéë èçìåíÿåòñÿ = 0

    return INI_OK;
    // ----------------
}




/*
    Óäàëÿåò èç îòêğûòîãî INI ôàéëà óêàçàííûé êëş÷ è åãî çíà÷åíèå.

    ÏÎÄĞÎÁÍÅÅ
        Ïàğñåğ èùåò â ÎÇÓ â êîíòåíòå ôàéëà óêàçàííûé êëş÷ è óäàëÿåò íàéäåííóş
        ïàğó êëş÷/çíà÷åíèå.

        Èìÿ êëş÷à/çíà÷åíèå â ôàéëå ìîæåò áûòü ëşáîé äëèíû,
        à òàêæå ìîæåò ñîäåğæàòü ëşáûå ñèìâîëû, êğîìå 2 ñèìâîëîâ êîíöà ñòğîêè \r è \n.

        Åñëè ıòè ñèìâîëû åñòü â èìåíè êëş÷à èëè â çíà÷åíèè,
        âû äîëæíû äîëæíû õîğîøî çíàòü è îñîçíàâàòü ïîñëåäñòâèÿ.

    ÏÀĞÀÌÅÒĞÛ:
        nFilePointer        ÈÄ_îòêğûòîãî_ôàéëà, ïîëó÷åííûé îò ini_openFile / ini_createFile
        szKeyName[]         èìÿ êëş÷à

    ÂÅĞÍÅÒ:
        êîä îøèáêè < 0 èëè 0 ïğè óñïåõå
*/

stock ini_removeKey ( nFilePointer, szKeyName[] )
{
    // ----------------
    if // åñëè ÈÄ îòêğûòîãî ôàéëà óêàçàí íåâåğíî
    (
            nFilePointer < 0
        ||  nFilePointer >= INI_MAX_OPENED_FILES
        ||  _ini_nSlotUsed[nFilePointer] != 1
    )
        return INI_WRONG_SLOT; // âåğíóòü êîä: íåâåğíûé óêàçàòåëü íà îòêğûòûé ôàéë
    // ----------------


    // ----------------
    new nKeyLen = strlen(szKeyName); // óçíàåì äëèíó èìåíè óêàçàííîãî êëş÷à

    if ( nKeyLen <= 0 ) // åñëè óêàçàí ïóñòîé êëş÷
        return INI_KEY_NOT_FOUND;
    // ----------------


    // ----------------
    for // ïåğåáîğ è ñğàâíåíèå âñåõ êëş÷åé ôàéëà ñ óêàçàííûì êëş÷îì
    (
        new kPos = 0, curFilePos, found;
        kPos < _ini_nKeysCount[nFilePointer];
        kPos++
    )
    {
        // ----------------
        found = 0; // ôëàã, íàéäåíà ëè ïîçèöèÿ êîíöà êëş÷à = 0

        for // èùåì ïîçèöèş êîíöà êëş÷à
        (
            curFilePos = _ini_nDelimPos[nFilePointer][kPos] - 1; // òåêóù. ïîç. ôàéëà = ïîç. òåêóù. INI_DELIMITER - 1
            curFilePos >= 0; // ïğîäîëæàòü ïîêà ïîç. ôàéëà >= 0
            curFilePos-- // ïîñëå êàæäîãî ïîâòîğà òåêóù. ïîç. ôàéëà -= 1
        )
        {
            switch ( _ini_szFileContent[nFilePointer]{curFilePos} ) // óçíàåì ÷òî çà ñèìâîë â òåêóù. ïîç. ôàéëà
            {
                case INI_SPACE_CHAR :           continue; // åñëè ıòî ïğîáåëüíûé ñèìâîë, ïåğåéäåì ê ïğåä. ñèìâîëó ôàéëà
                case INI_STRING_DELIMITER :     break; // åñëè ıòî êîíåö ñòğîêè
                default : // åñëè ıòî äğóãîé ñèìâîë
                {
                    found = 1; // ïîçèöèÿ êîíöà êëş÷à íàéäåíà
                    break; // êîíåö öèêëà
                }
            }
        }

        // åñëè ïîçèöèÿ êîíöà êëş÷à íå íàéäåíà, ïåğåõîä ê ñëåä. ïîçèöèè INI_DELIMITER
        if ( found != 1 ) continue;
        // ----------------


        // ñğàâíèâàåì ïîñèìâîëüíî òåêóùèé êëş÷ ôàéëà è óêàçàííûé êëş÷
        for ( new curKeyPos = nKeyLen - 1;  curKeyPos >= 0;  curFilePos--, curKeyPos-- )
        {
            if
            (
                    curFilePos < 0 // åñëè ïîç ôàéëà ñòàëà < 0
                ||  _ini_szFileContent[nFilePointer]{curFilePos} != szKeyName[curKeyPos] // åñëè ñèìâîëû èç êëş÷åé íå ğàâíû
                ||  _ini_szFileContent[nFilePointer]{curFilePos} == INI_STRING_DELIMITER // åñëè ñèìâîë èç êëş÷à ıòî INI_STRING_DELIMITER
            )
            {
                found = 0; // ôëàã, êëş÷ íå íàéäåí
                break; // êîíåö ñğàâíåíèÿ
            }
        }


        if ( found != 0 ) // åñëè óêàçàííûé êëş÷ ÍÀÉÄÅÍ â ôàéëå
        {
            // åñëè íàéäåíî ñîâïàäåíèå íå öåëîãî êëş÷à ôàéëà, à åãî îêîí÷àíèÿ ñ óêàçàííûì êëş÷îì
            if ( curFilePos >= 0 )
            {
                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_KEY_STARTS : {}
                    default: continue;
                }
            }


            // ñîõğàíèì ïîçèöèş íà÷àëà êëş÷à
            new keyStartPos = ( curFilePos <= 0 ) ? 0 : curFilePos + 1;


            // ----------------
            // òåêóùàÿ ïîçèöèÿ â ôàéëå áóäåò íà 1 áîëüøå òåêóùåé ïîçèöèè INI_DELIMITER
            curFilePos = _ini_nDelimPos[nFilePointer][kPos] + 1;

            // èùåì ïîçèöèş íà÷àëà çíà÷åíèÿ, îíà áóäåò ïîìåùåíà â curFilePos
            for ( ; ; curFilePos++ )
            {
                if ( curFilePos >= _ini_nFileSize[nFilePointer] ) break;

                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_SPACE_CHAR :   continue; // åñëè ıòî ïğîáåëüíûé ñèìâîë, ïåğåéäåì ê ñëåä. ñèìâîëó
                    default :               break; // åñëè ıòî äğóãîé ñèìâîë
                }
            }
            // ----------------


            // ----------------
            // èùåì ïîçèöèş êîíöà çíà÷åíèÿ, îíà áóäåò ïîìåùåíà â curFilePos
            for ( ; ; curFilePos++ )
            {
                if ( curFilePos >= _ini_nFileSize[nFilePointer] ) break;

                switch ( _ini_szFileContent[nFilePointer]{curFilePos} )
                {
                    case INI_STRING_ENDS :  break; // åñëè ıòî êîíåö ñòğîêè - ñòîï
                    default :               continue; // åñëè ıòî äğóãîé ñèìâîë, ïåğåéäåì ê ñëåä. ñèìâîëó
                }
            }

            // âû÷èñëèì ñìåùåíèå îñòàâøèõñÿ ïîçèöèé äëÿ INI_DELIMITER ïîñëå èçìåíåíèÿ çíà÷åíèÿ
            new filePosOffset = keyStartPos - curFilePos;
            // ----------------


            if ( _ini_nFileBusy[nFilePointer] != 0 ) // åñëè ïğÿìî ñåé÷àñ ôàéë èçìåíÿåòñÿ äğóãîé ôóíêöèåé
                return INI_WRITE_ERROR; // âåğíåì êîä îøèáêè ïğè çàïèñè â ôàéë


            // ----------------
            _ini_nFileBusy[nFilePointer] = 1; // ôëàã: ôàéë èçìåíÿåòñÿ = 1

            // çàòèğàåì êîíòåíò êëş÷à, ñëåäóşùèìè çà íèì äàííûìè ôàéëà
            for ( ; curFilePos <= _ini_nFileSize[nFilePointer]; curFilePos++ )
                _ini_szFileContent[nFilePointer]{ curFilePos + filePosOffset } =
                    _ini_szFileContent[nFilePointer]{curFilePos};

            // çàòèğàåì òåêóùèé INI_DELIMITER, è ìåíÿåì ïîçèöèè ïîñëåäóşùèõ INI_DELIMITER
            for ( ;  kPos < _ini_nKeysCount[nFilePointer];  kPos++ )
                _ini_nDelimPos[nFilePointer][kPos] = _ini_nDelimPos[nFilePointer][kPos + 1] + filePosOffset;

            _ini_nKeysCount[nFilePointer]--;
            _ini_nFileSize[nFilePointer] += filePosOffset; // èçìåíèì ğàçìåğ ôàéëà
            _ini_szFileContent[nFilePointer]{ _ini_nFileSize[nFilePointer] } = 0; // êîíåö ñòğîêè äëÿ êîíöà ôàéëà

            _ini_nFileChanged[nFilePointer]     = 1; // ôëàã: ôàéë èçìåíåí = 1
            _ini_nFileBusy[nFilePointer]        = 0; // ôëàã: ôàéë èçìåíÿåòñÿ = 0

            return INI_OK;
            // ----------------
        }
    }
    // ----------------


    //
    // åñëè óêàçàííûé êëş÷ ÍÅ ÍÀÉÄÅÍ â ôàéëå
    //

    // ----------------
    return INI_KEY_NOT_FOUND;
    // ----------------
}




/*
    Âîçâğàùàåò ñòğîêó ñ îïèñàíèåì óêàçàííîãî êîäà îøèáêè íà ğóññêîì ÿçûêå.

    ÏÎÄĞÎÁÍÅÅ
        Åñëè êàêàÿ-òî ôóíêöèÿ âàì âåğíóëà íå 0 (âñ¸ â ïîğÿäêå), à îòğèöàòåëüíîå
        çíà÷åíèå, çíà÷èò, èìåëà ìåñòî îøèáêà. Åñëè âû õîòèòå âûâåñòè â ëşáîé ëîã èëè
        â äèàëîã ÷åëîâå÷åñêîå îïèñàíèå ıòîé îøèáêè, òî ïåğåäàéòå ıòîé ôóíêöèè êîä
        îøèáêè è îíà âàì âåğíåò îïèñàíèå ıòîé îøèáêè íà ğóññêîì ÿçûêå.

        Îïèñàíèå îøèáîê äîâîëüíî äëèííûå è ïğåâûøàşò ğàçìåğ â 128 ñèìâîëîâ,
        ïîıòîìó âûâîäèòü ıòè îïèñàíèÿ â ÷àòå íå ğåêîìåíäóåòñÿ!

    ÏÀĞÀÌÅÒĞÛ:
        nErrorCode      êîä îøèáêè, ïîëó÷åííûé îò ëşáîé ôóíêöèè

    ÂÅĞÍÅÒ:
        ñòğîêó ñ ÷åëîâå÷åñêèì îïèñàíèåì îøèáêè, ïîä óêàçàííûì êîäîì

    ÏĞÈÌÅĞ:
        new ini = ini_openFile("íåñóùåñòâóşùèé ôàéë"); // ïûòàåìñÿ îòêğûòü íåñóùåñòâóşùèé ôàéë

        if ( ini < 0 ) print( ini_getErrorInfo(ini) );
        else
        {
            new number, result = ini_getInteger( ini, "íåñóùåñòâóşùèé êëş÷", number ); // ïğîáóåì ïîëó÷èòü çíà÷åíèå íåñóùåñòâóşùåãî êëş÷à

            if ( result < 0 ) print( ini_getErrorInfo(result) );

            ini_closeFile(ini);
        }
*/

stock ini_getErrorInfo ( nErrorCode )
{
    new error_ms[200] = "Îøèáîê íåò";

    if ( nErrorCode >= 0 ) return error_ms;

    switch ( nErrorCode )
    {
        case INI_FILE_NOT_FOUND :     error_ms = "INI ôàéë, óêàçàííûé â `ini_openFile`, íå ñóùåñòâóåò, âîçìîæíî, óêàçàí íåâåğíûé ïóòü ê íåìó";
        case INI_FILE_ALREADY_EXIST : error_ms = "INI ôàéë, óêàçàííûé â `ini_createFile`, óæå ñóùåñòâóåò, óêàæèòå äğóãîå èìÿ/ïóòü ê ôàéëó";
        case INI_TOO_LARGE_FILE :     error_ms = "Íåâîçìîæíî äîáàâèòü íîâûé êëş÷ â INI ôàéë, ò.ê. ğàçìåğ INI ôàéëà ïğåâûñèò äîïóñòèìûé ëèìèò â " #INI_MAX_FILE_SIZE " ñèìâîë(îâ), èëè áóäåò ïğåâûøåí ëèìèò â " #INI_MAX_KEYS_IN_FILE " êëş÷åé";
        case INI_WRONG_PATH_SIZE :    error_ms = "Äëèíà ïóòè ê INI ôàéëó, óêàçàííîãî â `ini_openFile` / `ini_createFile`, ïğåâûøàåò ëèìèò â " #INI_MAX_FILENAME_SIZE " ñèìâîë(îâ)";
        case INI_READ_ERROR :         error_ms = "Îøèáêà ïğè ÷òåíèè INI ôàéëà ñ äèñêà, âîçìîæíî, ôàéë çàíÿò äğóãèì ïğîöåññîì";
        case INI_WRITE_ERROR :        error_ms = "Îøèáêà ïğè çàïèñè ñîäåğæèìîãî INI ôàéëà íà äèñê, âîçìîæíî, ôàéë çàíÿò äğóãèì ïğîöåññîì";
        case INI_NO_FREE_SLOT :       error_ms = "Ëèìèò â " #INI_MAX_OPENED_FILES " îäíîâğåìåííî îòêğûòûõ INI ôàéëîâ èñ÷åğïàí, ïîıòîìó îòêğûòü/ñîçäàòü åùå 1 ôàéë íåëüçÿ, ïîêà íå çàêğûò õîòü 1 èç îòêğûòûõ ôàéëîâ";
        case INI_WRONG_SLOT :         error_ms = "Óêàçàí íåâåğíûé ID îòêğûòîãî INI ôàéëà, âîçìîæíî, ïğè îòêğûòèè INI ôàéëà ïğîèçîøëà îøèáêà, ïîıòîìó âìåñòî ID ôàéëà âû ïîëó÷èëè êîä îøèáêè";
        case INI_KEY_NOT_FOUND :      error_ms = "Óêàçàííûé êëş÷ íå íàéäåí â îòêğûòîì INI ôàéëå";
        case INI_WRONG_RETURN_SIZE :  error_ms = "Ïğè ïîïûòêå ÷òåíèÿ ñòğîêîâîãî çíà÷åíèÿ èç INI ôàéëà, ïîñëåäíèé ïàğàìåòğ `nSizeOfReturn` â `ini_getString` îêàçàëñÿ <= 0, óêàæèòå âğó÷íóş çíà÷åíèå > 0 äëÿ íåãî";

        default:                      error_ms = "Íåèçâåñòíàÿ îøèáêà";
    }

    return error_ms;
}
