#include "cell_locator.h"

VMUINT8* screenbuf = NULL;	
VMINT ncharacter_height = -1;
VMINT nscreen_width = -1;
VMINT nscreen_height = -1;
VMINT filledDsplByLines = 0;
VMBOOL third = VM_FALSE;
VMBOOL fix_pos = VM_FALSE;

static VMINT timer_id = -1;
VMBOOL flightMode = VM_FALSE;

VMWCHAR file_pathw[100] = {0};
VMWCHAR tmp_pathw[100] = {0};

//------------------
#define MAX_AUTOLOCATION_LEN   96

static char *db_buf = NULL;

static VMBOOL showCellID = VM_FALSE; //showCellID = false; // jei false rodo teksta, kitaip skaicius
VMBOOL AutoLocation = VM_TRUE; //jei true bus i db irasoma nauja reiksme
VMUINT currentLAC;
VMUINT currentCID;
VMWCHAR CellNameStatus[256]={0,}; //CellNameStatus = L"Home 1" (jei showCellID == false) arba laokacijos pavadinimas arba [LAC:CID]
VMWCHAR CellName[256] = {0,}; //CellName = L"Home 1"
VMWCHAR SIwstr[MAX_AUTOLOCATION_LEN] = {0,};

VMCHAR CellNameStatusZ[256]={0,};
//VMCHAR CellNameZ[256]={0,};

VMFILE f;

struct vm_time_t curr_time;

VMINT LocFound = 0; //1 jei vieta bazeje rasta arba prideta, 0 jei ne

VMWCHAR editor_title[100];

void vm_main(void) {

	layer_hdl[0] = -1;	
	vm_reg_sysevt_callback(handle_sysevt);
	vm_reg_keyboard_callback(handle_keyevt);

        vm_font_set_font_size(VM_SMALL_FONT);
        ncharacter_height = vm_graphic_get_character_height();
        nscreen_width = vm_graphic_get_screen_width();
        nscreen_height = vm_graphic_get_screen_height();

        if (vm_sim_card_count() == 99) {
           flightMode = VM_TRUE;
        } else {
	   create_app_txt_path(file_pathw, (char*)"txt");
	   create_app_txt_path(tmp_pathw, (char*)"tmp");
           vm_ascii_to_ucs2(editor_title, 42, "Input location name:");
           vm_input_set_editor_title(editor_title);
	   vm_cell_open();
           load_db_file();
	   timer_id = vm_create_timer_ex(1000, timer);
        }

}

void handle_sysevt(VMINT message, VMINT param) {

	switch (message) {
	case VM_MSG_CREATE:
	case VM_MSG_ACTIVE:
		layer_hdl[0] = vm_graphic_create_layer(0, 0, vm_graphic_get_screen_width(), vm_graphic_get_screen_height(), -1);
		vm_graphic_set_clip(0, 0, vm_graphic_get_screen_width(), vm_graphic_get_screen_height());
                fill_white();
		break;
		
	case VM_MSG_PAINT:
                vm_switch_power_saving_mode(turn_off_mode);
	        screenbuf = vm_graphic_get_layer_buffer(layer_hdl[0]);
                if (flightMode == VM_TRUE) {
                   fill_white();
                   display_text_line(screenbuf, "Please turn Flight mode off !", 0, filledDsplByLines, nscreen_width, nscreen_height, 2, 1, VM_COLOR_BLACK, VM_FALSE);
                }
		break;
		
	case VM_MSG_INACTIVE:
		if( layer_hdl[0] != -1 )
			vm_graphic_delete_layer(layer_hdl[0]);
		
		break;	
	case VM_MSG_QUIT:
		if( layer_hdl[0] != -1 )
			vm_graphic_delete_layer(layer_hdl[0]);
		
		break;	
	}
}

void handle_keyevt(VMINT event, VMINT keycode) {

        //char t[24] = {0};

	if (event == VM_KEY_EVENT_UP && keycode == VM_KEY_RIGHT_SOFTKEY) {
           if (layer_hdl[0] != -1) {
              vm_graphic_delete_layer(layer_hdl[0]);
              layer_hdl[0] = -1;
           }
        vm_exit_app();
	}

	if (event == VM_KEY_EVENT_UP && keycode == VM_KEY_LEFT_SOFTKEY) {
           if (flightMode == VM_FALSE) { timer_id = vm_create_timer_ex(1000, timer); }
	}

	if (event == VM_KEY_EVENT_UP && keycode == VM_KEY_NUM1) {
           if (flightMode == VM_FALSE && LocFound == 1) {
              //vm_get_time(&curr_time);
              //sprintf(t, "Work %02d.%02d %02d:%02d:%02d", curr_time.mon, curr_time.day, curr_time.hour, curr_time.min, curr_time.sec);
              vm_input_text3(0, 100, 0, job);
              //UpdateCellName(t);
              //timer_id = vm_create_timer_ex(1000, timer);
           }
	}

	if (event == VM_KEY_EVENT_UP && keycode == VM_KEY_NUM2) {
           if (flightMode == VM_FALSE) {
              if (showCellID == VM_FALSE) {
                 showCellID = VM_TRUE;
              } else {
                 showCellID = VM_FALSE;
              }
              timer_id = vm_create_timer_ex(1000, timer);
           }
	}

}

void timer(int a){

        int i;
        char t[20] = {0};
        char s[1000] = {0};
        char x[1000] = {0};
        //struct vm_time_t curr_time;

        vm_cell_info_struct* info = vm_cell_get_cur_cell_info(); //gauti prisijungtos stoties informacija
	//vm_cell_info_struct** bs = vm_cell_get_nbr_cell_info();  //gauti kaimyniniu stociu informacija

        vm_get_time(&curr_time);

        sprintf(t, "%04d-%02d-%02d %02d:%02d:%02d", curr_time.year, curr_time.mon, curr_time.day, curr_time.hour, curr_time.min, curr_time.sec);
        display_text_line(screenbuf, t, 0, filledDsplByLines, nscreen_width, nscreen_height, 2, 1, VM_COLOR_BLACK, VM_FALSE);

        //sprintf(x, "cellid:%d, lac:%d, signal:%d", info->ci, info->lac, info->rxlev-110);
        //display_text_line(screenbuf, x, 0, filledDsplByLines, nscreen_width, nscreen_height, 2, 1, VM_COLOR_BLACK, VM_FALSE);
        //display_text_line(screenbuf, "HHHHHHHHHHHHHHHHHHHHH", 0, filledDsplByLines, nscreen_width, nscreen_height, 2, 1, VM_COLOR_BLACK, VM_FALSE);

        //currentLAC = info->lac;
        //currentCID = info->ci;

        currentLAC = info->ci;
        currentCID = info->lac;

        CheckCurrentCell();

        vm_ucs2_to_ascii(CellNameStatusZ, sizeof(CellNameStatusZ), CellNameStatus);
        //vm_ucs2_to_ascii(CellNameZ, sizeof(CellNameZ), CellName);

        display_text_line(screenbuf, CellNameStatusZ, 0, filledDsplByLines, nscreen_width, nscreen_height, 2, 1, VM_COLOR_BLACK, VM_FALSE);
        //display_text_line(screenbuf, CellNameZ, 0, filledDsplByLines, nscreen_width, nscreen_height, 2, 1, VM_COLOR_BLACK, VM_FALSE);

	//int n = *vm_cell_get_nbr_num();

	//for(i = 0; i<n; ++i)
	//{
	//	sprintf(s, "cellid:%d, lac:%d, signal:%d", bs[i]->ci, bs[i]->lac, bs[i]->rxlev-110);
        //        display_text_line(screenbuf, s, 0, filledDsplByLines, nscreen_width, nscreen_height, 2, 1, VM_COLOR_BLACK, VM_FALSE);
	//}

	if (timer_id != -1) {
            vm_delete_timer_ex(timer_id);
            timer_id = -1;
        }

        //vm_cell_close();
}

static void fill_white(void) {

	vm_graphic_color color;

	color.vm_color_565 = VM_COLOR_WHITE;
	vm_graphic_setcolor(&color);
	vm_graphic_fill_rect_ex(layer_hdl[0], 0, 0, vm_graphic_get_screen_width(), vm_graphic_get_screen_height());
	vm_graphic_flush_layer(layer_hdl, 1);
        filledDsplByLines = 0;

}

VMINT string_width(VMWCHAR *whead, VMWCHAR *wtail) {

	VMWCHAR * wtemp = NULL;
	VMINT width = 0;
	if (whead == NULL || wtail == NULL)
	return 0;
	wtemp = (VMWCHAR *)vm_malloc((wtail - whead) * 2 + 2);
	
	if (wtemp == NULL)
	return 0;
	memset(wtemp, 0, (wtail - whead) * 2 + 2);
	memcpy(wtemp, whead, (wtail - whead) * 2);

	width = vm_graphic_get_string_width(wtemp);
	vm_free(wtemp);
	return width;
}

void display_text_line(VMUINT8 *disp_buf, VMSTR str, VMINT x, VMINT y, VMINT width, VMINT height, VMINT betlines, VMINT startLine, VMINT color, VMBOOL fix_pos) {

	VMWCHAR *ucstr;
	VMWCHAR *ucshead;
	VMWCHAR *ucstail;
	VMINT is_end = FALSE;
	VMINT nheight = y; 
	VMINT nline_height ;
	VMINT nlines = 0;

        if (y = 0) {fill_white();}

	if (str == NULL||disp_buf==NULL||betlines < 0) {return;}

	nline_height = vm_graphic_get_character_height() + betlines;

        if (third == VM_TRUE && fix_pos == VM_FALSE){
           nheight = nheight + nline_height;
           third = VM_FALSE;
        }

	if (fix_pos == VM_TRUE) {
	   vm_graphic_fill_rect(screenbuf, 0, filledDsplByLines, nscreen_width, nline_height, VM_COLOR_WHITE, VM_COLOR_WHITE);
           third = VM_TRUE;
	}

	ucstr = (VMWCHAR*)vm_malloc(2*(strlen(str)+1));

	if (ucstr == NULL) {return;}
	
	if(0 != vm_ascii_to_ucs2(ucstr,2*(strlen(str)+1),str)) {
		vm_free(ucstr);
		return ;
	}
	ucshead = ucstr;
	ucstail = ucshead + 1;
	
	while(is_end == FALSE)
	{
		//if (nheight+nline_height>y+height) // paliekam paskutine ekrano eilute del meniu
		if (nheight > y + height) { // jeigu uzimtas ekrano aukstis virsijo 320
                        fill_white();
                        nheight = 0;
			//break;
                }

		while (1)
		{
			if (string_width(ucshead,ucstail)<=width)
			{
				ucstail ++;
			}
			else
			{
				nlines++;
				ucstail --;
				break;
			}
			if (0==vm_wstrlen(ucstail))
			{
				is_end = TRUE;
				nlines++;
				break;
			}
		}
		if ( nlines >= startLine)
		{
			vm_graphic_textout(disp_buf, x, nheight, ucshead, (ucstail-ucshead), (VMUINT16)(color));
                        vm_graphic_flush_layer(layer_hdl, 1);
			if (fix_pos == VM_FALSE) {nheight += nline_height;}
                        filledDsplByLines = nheight;
		}
		ucshead = ucstail;
		ucstail ++;
	}
	vm_free(ucstr);
}

void create_app_txt_path(VMWSTR text, VMSTR extt) {

    VMWCHAR fullPath[100];
    VMWCHAR wfile_extension[8];

    vm_get_exec_filename(fullPath);
    vm_ascii_to_ucs2(wfile_extension, 8, extt);
    vm_wstrncpy(text, fullPath, vm_wstrlen(fullPath) - 3);
    vm_wstrcat(text, wfile_extension);

}

// Load DB file into memory
char *load_db_file() {

    VMFILE f;
    VMUINT nread;
    VMINT sz;
    //VMWCHAR file_pathw[100];

    //create_app_txt_path(file_pathw, (char*)"txt");
    f = vm_file_open(file_pathw, MODE_READ, FALSE);

    if (f < 0) {
        if (db_buf) { vm_free(db_buf); db_buf = NULL; }
        return NULL;
    }

    if (vm_file_getfilesize(f, &sz) < 0) {
        vm_file_close(f);
        if (db_buf) { vm_free(db_buf); db_buf = NULL; }
        return NULL;
    }

    char *buffer = (char*)vm_malloc(sz + 1);
    if (!buffer) {
        vm_file_close(f);
        if (db_buf) { vm_free(db_buf); db_buf = NULL; }
        return NULL;
    }

    vm_file_read(f, buffer, sz, &nread);
    buffer[nread] = '\0';
    vm_file_close(f);

    if (db_buf) { vm_free(db_buf); db_buf = NULL; }
    db_buf = buffer;

    return db_buf;
}

int CheckCurrentCell() {

    VMCHAR cellNameAscii[64] = {0};
    VMCHAR lacCidKey[64];

    char t[24] = {0};
    //struct vm_time_t curr_time;

    // Sudarome LAC:CID tekstą ASCII formatu
    sprintf(cellNameAscii, "%05d:%05d", (int)currentLAC, (int)currentCID);

    // Konvertuojame į UCS2
    vm_ascii_to_ucs2(CellName, sizeof(CellName), cellNameAscii);
    vm_wstrcpy(CellNameStatus, CellName);

    // Rakto forma duomenų bazėje: [LAC:CID]
    sprintf(lacCidKey, "[%s]", cellNameAscii);

    //VMINT LocFound = 0;

    if (db_buf) {
        VMCHAR *param = strstr(db_buf, lacCidKey);
        if (param) {
            // Raskime pavadinimą po "]:"
            VMCHAR *start = strstr(param, "]:");
            if (start) {
                start += 2; // praleidžiame "]:"
                VMCHAR *end = strchr(start, '\r');
                if (!end) end = strchr(start, '\n'); // jei tik LF
                if (end) *end = '\0';

                vm_ascii_to_ucs2(CellName, sizeof(CellName), start);
                if (!showCellID)
                    vm_wstrcpy(CellNameStatus, CellName);
            }
            LocFound = 1;
        }
    }

    // Jei nerasta ir AutoLocation = TRUE → pridėti naują įrašą
    if (!LocFound && AutoLocation == VM_TRUE) {
        if (vm_wstrlen(SIwstr) == 0)
            vm_get_time(&curr_time);
            sprintf(t, "Unknown %02d.%02d %02d:%02d:%02d", curr_time.mon, curr_time.day, curr_time.hour, curr_time.min, curr_time.sec);
            vm_ascii_to_ucs2(SIwstr, sizeof(SIwstr), t);
        if (AddTo_db(SIwstr)) LocFound = 1;
    }

    return LocFound;
}


int AddTo_db(VMWSTR wstr) {

    if (!wstr) return 0;

    VMCHAR asciiValue[256] = {0};
    VMCHAR newEntry[512] = {0};
    VMCHAR cellKey[32];
    VMINT status = 0;
    VMUINT nwrite;

    // UCS2 → ASCII
    vm_ucs2_to_ascii(asciiValue, sizeof(asciiValue), wstr);

    // Sukuriame įrašo tekstą: [LAC:CID]:Name
    sprintf(newEntry, "[%05d:%05d]:%s\r\n", (int)currentLAC, (int)currentCID, asciiValue);
    sprintf(cellKey, "[%05d:%05d]", (int)currentLAC, (int)currentCID);

    // Užkrauname DB, jei dar neužkrauta
    if (!db_buf) load_db_file();

    VMCHAR *p1 = db_buf ? strstr(db_buf, cellKey) : NULL;

    if (p1) {
        // Pakeičiame esamą įrašą
        VMFILE tf = vm_file_open(tmp_pathw, MODE_CREATE_ALWAYS_WRITE, FALSE); // ASCII režimas
        if (tf >= 0) {
            // Rašome iki rastos pozicijos
            vm_file_write(tf, db_buf, (p1 - db_buf), &nwrite);

            // Rašome naują įrašą
            if (*wstr)
                vm_file_write(tf, newEntry, strlen(newEntry), &nwrite);

            // Rašome likusią dalį po seno įrašo
            VMCHAR *p2 = strstr(p1 + 1, "[");
            if (p2)
                vm_file_write(tf, p2, strlen(p2), &nwrite);

            vm_file_close(tf);

            // Perrašome originalų failą
            vm_file_delete(file_pathw);
            vm_file_rename(tmp_pathw, file_pathw);
            status = 1;
        }
    } else {
        // Pridedame naują įrašą prie failo pabaigos
        if (*wstr) {
            VMFILE f = vm_file_open(file_pathw, MODE_WRITE, FALSE);
            if (f < 0){ f = vm_file_open(file_pathw, MODE_CREATE_ALWAYS_WRITE, FALSE); }
            if (f >= 0) {
                vm_file_seek(f, 0, BASE_END);
                vm_file_write(f, newEntry, strlen(newEntry), &nwrite);
                vm_file_close(f);
                status = 1;
            }
        }
    }

    // Perkrauname DB buferį
    load_db_file();

    if (!showCellID)
        vm_wstrcpy(CellNameStatus, wstr);

    return status;
}



void update_current_cell_info() {

    vm_cell_info_struct *cell = vm_cell_get_cur_cell_info();
    if (cell != NULL) {
        currentLAC = cell->lac;
        currentCID = cell->ci;
    } else {
        currentLAC = 0;
        currentCID = 0;
    }
}

void MiniGPS_Update() {

    char t[20] = {0};
    char x[100] = {0};
    vm_cell_info_struct *cell;
    struct vm_time_t curr_time;

    vm_get_time(&curr_time);
    sprintf(t, "%04d-%02d-%02d %02d:%02d:%02d", curr_time.year, curr_time.mon, curr_time.day, curr_time.hour, curr_time.min, curr_time.sec);
    display_text_line(screenbuf, t, 0, filledDsplByLines, nscreen_width, nscreen_height, 2, 1, VM_COLOR_BLACK, VM_FALSE);

    if (vm_cell_open() >= 0) {
        cell = vm_cell_get_cur_cell_info();
        if (cell) {
            currentLAC = cell->lac;
            currentCID = cell->ci;
            //CheckCurrentCell();  // atnaujina pavadinimą pagal DB

            sprintf(x, "cellid:%d, lac:%d, signal:%d", cell->ci, cell->lac, cell->rxlev-110);
            display_text_line(screenbuf, x, 0, filledDsplByLines, nscreen_width, nscreen_height, 2, 1, VM_COLOR_BLACK, VM_FALSE);

        } else {
            display_text_line(screenbuf, "MiniGPS: cell info not available.", 0, filledDsplByLines, nscreen_width, nscreen_height, 2, 1, VM_COLOR_BLACK, VM_FALSE);
        }
        vm_cell_close();
    } else {
        display_text_line(screenbuf, "MiniGPS: cannot open cell module.", 0, filledDsplByLines, nscreen_width, nscreen_height, 2, 1, VM_COLOR_BLACK, VM_FALSE);
    }
}

void calibrate_cell_order(void) {

    vm_cell_info_struct *info = vm_cell_get_cur_cell_info();
    if (info) {
        // Jei ci atrodo mažas (pvz. < 10000), o lac didelis — sukeičiam
        if (info->ci < 10000 && info->lac > 10000) {
            currentLAC = info->lac;
            currentCID = info->ci;
        } else {
            currentLAC = info->ci;
            currentCID = info->lac;
        }
    }
}

void UpdateCellName(char *newName) {

    VMWCHAR wname[64];
    vm_ascii_to_ucs2(wname, sizeof(wname), newName);
    AddTo_db(wname);
}

void job(VMINT state, VMWSTR text) {

    int lenght;
    lenght = wstrlen(text);
    VMCHAR t[100];

    if (state == VM_INPUT_OK && lenght > 0) {
       vm_ucs2_to_ascii(t, lenght + 1, text);
       UpdateCellName(t);
       timer_id = vm_create_timer_ex(1000, timer);
    }

}