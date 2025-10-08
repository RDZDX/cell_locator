#ifndef _VRE_APP_WIZARDTEMPLATE_
#define	_VRE_APP_WIZARDTEMPLATE_

#include "vmsys.h"
#include "vmio.h"
#include "vmgraph.h"
#include "vmchset.h"
#include "vmstdlib.h"
#include "vmcell.h"
#include "vmtimer.h"
#include "string.h"
#include "stdint.h"
#include <time.h>

VMINT layer_hdl[2];

void handle_sysevt(VMINT message, VMINT param);
void handle_keyevt(VMINT event, VMINT keycode);
void timer(int a);
static void fill_white(void);
VMINT string_width(VMWCHAR *whead, VMWCHAR *wtail);
void display_text_line(VMUINT8 *disp_buf, VMSTR str, VMINT x, VMINT y, VMINT width, VMINT height, VMINT betlines, VMINT startLine, VMINT color, VMBOOL fix_pos);
void create_app_txt_path(VMWSTR text, VMSTR extt);
char *load_db_file();
int AddTo_db(VMWSTR wstr);
int CheckCurrentCell();
void update_current_cell_info();
void MiniGPS_Update();
VMINT my_wsprintf(VMWSTR out, VMINT size, const VMWSTR fmt, ...);
void calibrate_cell_order(void);
void UpdateCellName(char *newName);
void save_text(VMINT state, VMWSTR text);
void display_text_line1(
    VMUINT8 *disp_buf,
    VMWSTR str,
    VMINT x,
    VMINT y,
    VMINT width,
    VMINT height,
    VMINT betlines,
    VMINT startLine,
    VMINT color,
    VMBOOL fix_pos
);
VMINT string_width1(VMWCHAR *whead, VMWCHAR *wtail);

#endif

