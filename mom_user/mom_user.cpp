//////////////////////////////////////////////////////////////////////////////
//
//  mom_user.cpp
//
//  Description:
//      Contains Unigraphics entry points for the application.
//
//////////////////////////////////////////////////////////////////////////////

// #define _CRT_SECURE_NO_DEPRECATE 1

//  Include files
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <uf.h>
#include <uf_exit.h>
#include <uf_ui.h>
#include <uf_mom.h>

#include <windows.h>

#include "mom_user.h"

//----------------------------------------------------------------------------

/*
 * Защита через номер тома:
 */
int sub_10002920() ;
/*
 * Exported Tcl procedures:
 */
static int sub_10001090
(
  void *client_data,    /* This is the mom object */
  void *interp,         /* Pointer to the TCL interpreter */
  int  argc,            /* Count of the numnber of arguments */
  char **argv           /* Array of arguments where the
                           argv[0] - The TCL extension */
);

//----------------------------------------------------------------------------
//  Activation Methods
//----------------------------------------------------------------------------

//  Explicit Activation
//      This entry point is used to activate the application explicitly, as in
//      "File->Execute UG/Open->User Function..."
//----- (10001010) --------------------------------------------------------
extern "C" DllExport void entry_rtv_mom_cp_part_attr( char *parm, int *returnCode, int rlen )
{
  int errorCode ;
  UF_MOM_id_t mom ;
  void *interp = NULL ;

  if ( !UF_is_initialized() )
  {
    errorCode = UF_initialize();
    *returnCode = errorCode;
    if ( errorCode ) return ;
  }

  /* Get the TCL interpreter id from the ufusr param */
  UF_MOM_ask_interp_from_param ( parm, &interp ) ;
  /* Get the MOM id from the ufusr param */
  UF_MOM_ask_mom (parm, &mom) ;
  /*Создаем функции в интепретаторе*/
  UF_MOM_extend_xlator ( mom , "EXTN_rtv_mom_cp_part_attr", sub_10001090 ) ;

  *returnCode = 0;
  if ( errorCode == 1 )  errorCode = UF_terminate();
  *returnCode = errorCode;
}


//----------------------------------------------------------------------------
//  Utilities
//----------------------------------------------------------------------------

// Unload Handler
//     This function specifies when to unload your application from Unigraphics.
//     If your application registers a callback (from a MenuScript item or a
//     User Defined Object for example), this function MUST return
//     "UF_UNLOAD_UG_TERMINATE".
extern "C" int ufusr_ask_unload( void )
{
     /* unload immediately after application exits*/
     //return ( UF_UNLOAD_IMMEDIATELY );

     /*via the unload selection dialog... */
     //return ( UF_UNLOAD_SEL_DIALOG );
     /*when UG terminates...              */
     return ( UF_UNLOAD_UG_TERMINATE );
}


/*-----------------------------------------------------------------------
 * Function Name: sub_10001090
 *
 *
 -----------------------------------------------------------------------*/
static int sub_10001090
(
  void *client_data,    /* This is the mom object */
  void *interp,         /* Pointer to the TCL interpreter */
  int  argc,            /* Count of the numnber of arguments */
  char **argv           /* Array of arguments where the
                           argv[0] - The TCL extension */
)
{
  UF_MOM_id_t mom_id = ( UF_MOM_id_t ) client_data ;

  int     ii ;
  char    Dest[256] ;
  double  angle1 , angle2 ;
  double  prev_angle1, prev_angle2 ;
  double  x , y , z , i , j , k ;
  const char *tool_adjust_register , *tool_cutcom_register;
  int     tool, tool1, tool2 ;
  const char *kin_retract_distance ;
  double  retract_distance ;
  double  dx , dy , dz ;

  double  v1, v2 ;
  int dword_100034F4 ;

  if ( sub_10002920() == 0 )
  {
  const int Size = 1024 ;
    char  str[Size]="НА ЭТОМ РАБОЧЕМ МЕСТЕ ПОСТПРОЦЕССОР НЕ МОЖЕТ БЫТЬ ЗАПУЩЕН\n \
Москва, CSoft (ЗАО \"СиСофт\"),\nТелефон: +7 (495) 913-22-22, Факс: +7 (495) 913-22-21\n \
e-mail: jura@csoft.ru, g00@inbox.ru\nHTTP: http://www.csoft.ru\n\n(c) 2007.\n\n\nПоследняя редакция 2008\0";

    UF_UI_display_nonmodal_msg("Внимание!", str, UF_UI_MSG_POS_CURSOR);
    UF_terminate();
    return ( 0 ) ;
  }

    sprintf(Dest, "Number of args %ld", argc);
    if ( argc > 0 )
    {
      for (ii=0;ii<argc;ii++) sprintf(Dest, "Arg 0 %s", argv[ii]);
    }

    if (0==strcmp(argv[1],"PLANE_TURN"))
    {
      UF_MOM_ask_string(mom_id, "tool_adjust_register", &tool_adjust_register);
      tool = (int) atof(tool_adjust_register);
      tool1 = tool + 12000 ;
      tool2 = tool + 2000 ;
      // G52 X[#%d*2] Z[#%d+#751]
      sprintf(Dest,"MOM_output_literal \"G52 X\\133\\043%d*2\\135 Z\\133\\043%d+\\043751\\135\"", tool1, tool2);
      UF_MOM_execute_command(mom_id, Dest);
      sprintf(Dest,"MOM_output_literal \"D000\"");
      UF_MOM_execute_command(mom_id, Dest);
    }

    if (0==strcmp(argv[1],"PLANE_MILL"))
    {
      UF_MOM_ask_assoc_double_array(mom_id, "out_angle_pos", "0", &angle1);
      UF_MOM_ask_assoc_double_array(mom_id, "out_angle_pos", "1", &angle2);
      UF_MOM_ask_assoc_double_array(mom_id, "prev_out_angle_pos", "0", &prev_angle1);
      UF_MOM_ask_assoc_double_array(mom_id, "prev_out_angle_pos", "1", &prev_angle2);
      UF_MOM_ask_assoc_double_array(mom_id, "pos", "0", &x);
      UF_MOM_ask_assoc_double_array(mom_id, "pos", "1", &y);
      UF_MOM_ask_assoc_double_array(mom_id, "pos", "2", &z);
      UF_MOM_ask_string(mom_id, "tool_cutcom_register", &tool_cutcom_register);
      tool = (int) atof(tool_cutcom_register);
      tool = tool + 2000 ;
      v1 = cos(angle1 * 0.0174532925199433);
      v2 = sin(angle1 * 0.0174532925199433);
      // #601=[[#%d]*2*[%.7lf]]
      sprintf(
        Dest,
      "MOM_output_literal \"\\043601=\\133\\133\\043%d\\135*2*\\133%.7lf\\135\\135\"",
      tool, v2);
      UF_MOM_execute_command(mom_id, Dest);
      // #602=[[#%d]*[%.7lf]-[#%d]]
      sprintf(
        Dest,
        "MOM_output_literal \"\\043602=\\133\\133\\043%d\\135*\\133%.7lf\\135\\-\\133\\043%d\\135\\135\"",
        tool, v1, tool);
       // G52 X[#601] Z[#602+#751]
      sprintf(Dest, "MOM_output_literal \"G52 X\\133\\043601\\135 Z\\133\\043602+\\043751\\135\"");
      UF_MOM_execute_command(mom_id, Dest);
      sprintf(Dest, "MOM_output_literal \"G54\"");
      UF_MOM_execute_command(mom_id, Dest);
    }

    if (0==strcmp(argv[1],"PLANE_MILL2"))
    {
      UF_MOM_ask_assoc_double_array(mom_id, "out_angle_pos", "0", &angle1);
      UF_MOM_ask_assoc_double_array(mom_id, "out_angle_pos", "1", &angle2);
      UF_MOM_ask_assoc_double_array(mom_id, "prev_out_angle_pos", "0", &prev_angle1);
      UF_MOM_ask_assoc_double_array(mom_id, "prev_out_angle_pos", "1", &prev_angle2);
      UF_MOM_ask_assoc_double_array(mom_id, "pos", "0", &x);
      UF_MOM_ask_assoc_double_array(mom_id, "pos", "1", &y);
      UF_MOM_ask_assoc_double_array(mom_id, "pos", "2", &z);
      UF_MOM_ask_string(mom_id, "tool_cutcom_register", &tool_cutcom_register);
      tool = (int) atof(tool_cutcom_register);
      tool = tool + 2000 ;
      x = x + x ;
      sprintf( Dest, "MOM_output_literal \"G68.5 X0 Y0 Z0 I0 J1 K0 R%.3lf\"", angle1);
      UF_MOM_execute_command(mom_id, Dest);
      sprintf(Dest, "MOM_output_literal \"G0 X%.3lf Y%.3lf\"", x,y);
      UF_MOM_execute_command(mom_id, Dest);
    }

  if (0==strcmp(argv[1],"PLANE_moving2"))
    {
      UF_MOM_ask_assoc_double_array(mom_id, "pos", "0", &x);
      UF_MOM_ask_assoc_double_array(mom_id, "pos", "1", &y);
      UF_MOM_ask_assoc_double_array(mom_id, "pos", "2", &z);
      // G01 G54.2 P1 X%.3lf Y%.3lf G91 Z[-#800] F10000.
      sprintf( Dest, "MOM_output_literal \"G01 G54.2 P1 X%.3lf Y%.3lf G91 Z\\133-\\043800\\135 F10000. \"", x,y);
      UF_MOM_execute_command(mom_id, Dest);
    }

    if (0==strcmp(argv[1],"Recalc_Pos"))
    {
      UF_MOM_ask_assoc_double_array(mom_id, "pos", "0", &x);
      UF_MOM_ask_assoc_double_array(mom_id, "pos", "1", &y);
      UF_MOM_ask_assoc_double_array(mom_id, "pos", "2", &z);

      UF_MOM_set_assoc_double_array(mom_id, "calc_pos", "0", x);
      UF_MOM_set_assoc_double_array(mom_id, "calc_pos", "1", y);
      UF_MOM_set_assoc_double_array(mom_id, "calc_pos", "2", z);
    }

    if (0==strcmp(argv[1],"INPUT_RETRACT"))
    {
      int  ia4[2]={0,0},
           ip6[2] = {1,1},
           resp;
      double ra5[2] = { 100, 100 } ;
      char   menu[2][16]; //={":Отвод=",":Врезание="};
      strcpy(&menu[0][0], ":Отвод (мм)=");
      strcpy(&menu[1][0], ":Врезание (мм)=");
      //ra5[0] = *n0;
      //ra5[1] = *n1;

      UF_UI_close_listing_window();
      UF_UI_lock_ug_access(UF_UI_FROM_CUSTOM);
      resp = uc1608(":Введите значения отвода и врезания инструмента:", menu, 2, ia4, ra5, ip6);
        //*n0 = ra5[0];
        //*n1 = ra5[1];
      UF_MOM_set_double(mom_id, "kin_retract_distance", ra5[0]);
      UF_MOM_set_double(mom_id, "kin_reengage_distance", ra5[1]);
      UF_UI_unlock_ug_access(UF_UI_FROM_CUSTOM);
    }

    if (0==strcmp(argv[1],"INPUT_RETRACT_DIST"))
    {
      uc1601(":Ввод дистанций отвода и врезания:", 0);
      UF_MOM_ask_assoc_double_array(mom_id, "prev_mcs_goto", "0", &x);
      UF_MOM_ask_assoc_double_array(mom_id, "prev_mcs_goto", "1", &y);
      UF_MOM_ask_assoc_double_array(mom_id, "prev_mcs_goto", "2", &z);
      UF_MOM_ask_assoc_double_array(mom_id, "prev_tool_axis", "0", &i);
      UF_MOM_ask_assoc_double_array(mom_id, "prev_tool_axis", "1", &j);
      UF_MOM_ask_assoc_double_array(mom_id, "prev_tool_axis", "2", &k);
      UF_MOM_ask_string(mom_id, "kin_retract_distance", &kin_retract_distance);
      retract_distance=atof(kin_retract_distance);
      //UF_free(&kin_retract_distance);

      dx=i*retract_distance + x ;
      dy=j*retract_distance + y ;
      dz=k*retract_distance + z ;

      UF_MOM_set_assoc_double_array(mom_id, "pos", "0", dx);
      UF_MOM_set_assoc_double_array(mom_id, "pos", "1", dy);
      UF_MOM_set_assoc_double_array(mom_id, "pos", "2", dz);
    }

    if (0==strcmp(argv[1],"UNLOAD")) dword_100034F4 = 0;

    UF_MOM_set_string(mom_id, "var_unknow", Dest);
    UF_terminate();

  return ( 0 ) ;
}

//----- (10002920) --------------------------------------------------------
int sub_10002920()
{
  char VolumeNameBuffer[MAX_PATH]; // [sp+444h] [bp-3E8h]@1
  DWORD VolumeSerialNumber; // [sp+0h] [bp-82Ch]@1
  DWORD MaximumComponentLength; // [sp+54h] [bp-7D8h]@1
  DWORD FileSystemFlags; // [sp+58h] [bp-7D4h]@1
  char FileSystemNameBuffer[MAX_PATH]; // [sp+5Ch] [bp-7D0h]@1

  int i ;
  static DWORD sp[]=
  {  0x01 , 0x02 , 0x03 , 0x04 , 0x05 , 0x06 , 0x07 , 0x08 ,
     0x08 , 0x10 , 0x11 , 0x12 , 0x13 , 0x14 , 0x15 , 0x16 ,
     0x17 , 0x18 , 0x19 , 0x20 , 0x21 , 0x22 , 0x23 , 0x24 ,
     0x25 , 0x26 , 0x27 , 0x28 , 0x29 , 0x30 ,
     1 } ;

  GetVolumeInformation(
    "C:\\",
    VolumeNameBuffer,
    sizeof(VolumeNameBuffer),
    &VolumeSerialNumber,
    &MaximumComponentLength,
    &FileSystemFlags,
    FileSystemNameBuffer,
    sizeof(FileSystemNameBuffer));

  i = 0;
  while ( VolumeSerialNumber != sp[i] )
  {
    ++i;
    if ( i >= 31 ) return 0;
  }
  return 1;
}