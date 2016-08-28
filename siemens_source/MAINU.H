//---------------------------------------------------------------------------

#ifndef mainuH
#define mainuH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "UPlasmaForm.hpp"
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include "OvalBtn.hpp"
#include <Menus.hpp>
#include <Dialogs.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
//---------------------------------------------------------------------------
class Tmainfunctions : public TForm
{
__published:	// IDE-managed Components
        TPlasmaForm *PlasmaForm1;
        TOvalButton *OvalButton1;
        TOvalButton *OvalButton2;
        TOvalButton *OvalButton3;
        TOvalButton *OvalButton4;
        TOvalButton *OvalButton5;
        TOvalButton *OvalButton6;
        TPopupMenu *PopupMenu1;
        TMenuItem *Read1;
        TMenuItem *Unlocking1;
        TMenuItem *Decrypt1;
        TMenuItem *Dongle1;
        TMenuItem *Misc1;
        TMenuItem *Exit1;
        TMenuItem *ReadflashfromphoneandstoreasFLS1;
        TMenuItem *N1;
        TMenuItem *ReadflashfromphoneviaBFBandstoreasFLS1;
        TMenuItem *WriteFLSflashfiletophone1;
        TMenuItem *Createlogfromphone1;
        TMenuItem *Createmapfromlog1;
        TMenuItem *Createmapsfromlogs1;
        TMenuItem *Createmapfromlog2;
        TMenuItem *Directunlockandmapisalsosaved1;
        TMenuItem *Directunlockandmapisnotsaved1;
        TMenuItem *Createbackupmap1;
        TMenuItem *Loadmaptophone1;
        TMenuItem *ReadandpatchbootcoreinFLSformat1;
        TMenuItem *Decryptmapfromphone1;
        TMenuItem *Decryptmapfromfile1;
        TMenuItem *Testdongle1;
        TMenuItem *N2;
        TMenuItem *Getdongleinfo1;
        TMenuItem *Uploaddongleinfo1;
        TMenuItem *Updatedongle1;
        TMenuItem *N3;
        TMenuItem *N4;
        TMenuItem *Backupbatteryparametersfromphone1;
        TMenuItem *Restorebatteryparametersfromfile1;
        TMenuItem *Setdebuglevel1;
        TMenuItem *Low1;
        TMenuItem *Medium1;
        TMenuItem *High1;
        TMenuItem *SelectCOMport1;
        TMenuItem *COMport1app;
        TMenuItem *COMport2app;
        TMenuItem *COMport3app;
        TMenuItem *COMport4app;
        TMenuItem *SelectCOMportfordongle1;
        TMenuItem *COMport1dng;
        TMenuItem *COMport2dng;
        TMenuItem *COMport3dng;
        TMenuItem *COMport4dng;
        TMenuItem *COMport5dng;
        TMenuItem *COMport6dng;
        TMenuItem *COMport7dng;
        TMenuItem *OverrideCOMspeed1;
        TMenuItem *COMspeed57600;
        TMenuItem *COMspeed115200;
        TMenuItem *COMspeed230400;
        TMenuItem *COMspeed460800;
        TMenuItem *SetRedirection1;
        TMenuItem *redirect0;
        TMenuItem *redirect1;
        TMenuItem *redirect2;
        TMenuItem *Clearlogwindow1;
        TMenuItem *saveconfigurationonexit;
        TMenuItem *SaveConfiguration1;
        TMenuItem *N5;
        TMenuItem *N6;
        TOpenDialog *opendialog;
        TOvalButton *OvalButton7;
        TMenuItem *SelectCOMtypeforcable1;
        TMenuItem *comport_RTS;
        TMenuItem *comport_DTR;
        TMenuItem *boottype1;
        TMenuItem *boottype2;
        TMenuItem *boottype3;
        void __fastcall OvalButton6Click(TObject *Sender);
        void __fastcall FormActivate(TObject *Sender);
        void __fastcall OvalButton5Click(TObject *Sender);
        void __fastcall OvalButton4Click(TObject *Sender);
        void __fastcall OvalButton2Click(TObject *Sender);
        void __fastcall OvalButton1Click(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall Low1Click(TObject *Sender);
        void __fastcall Medium1Click(TObject *Sender);
        void __fastcall High1Click(TObject *Sender);
        void __fastcall redirect0Click(TObject *Sender);
        void __fastcall redirect1Click(TObject *Sender);
        void __fastcall redirect2Click(TObject *Sender);
        void __fastcall COMport1appClick(TObject *Sender);
        void __fastcall COMport2appClick(TObject *Sender);
        void __fastcall COMport3appClick(TObject *Sender);
        void __fastcall COMport4appClick(TObject *Sender);
        void __fastcall COMport1dngClick(TObject *Sender);
        void __fastcall COMport2dngClick(TObject *Sender);
        void __fastcall COMport3dngClick(TObject *Sender);
        void __fastcall COMport4dngClick(TObject *Sender);
        void __fastcall COMport5dngClick(TObject *Sender);
        void __fastcall COMport6dngClick(TObject *Sender);
        void __fastcall COMport7dngClick(TObject *Sender);
        void __fastcall COMspeed57600Click(TObject *Sender);
        void __fastcall Clearlogwindow1Click(TObject *Sender);
        void __fastcall SaveConfiguration1Click(TObject *Sender);
        void __fastcall Loadmaptophone1Click(TObject *Sender);
        void __fastcall COMspeed115200Click(TObject *Sender);
        void __fastcall COMspeed460800Click(TObject *Sender);
        void __fastcall COMspeed230400Click(TObject *Sender);
        void __fastcall Createmapfromlog2Click(TObject *Sender);
        void __fastcall Directunlockandmapisalsosaved1Click(
          TObject *Sender);
        void __fastcall Directunlockandmapisnotsaved1Click(
          TObject *Sender);
        void __fastcall Createbackupmap1Click(TObject *Sender);
        void __fastcall ReadflashfromphoneandstoreasFLS1Click(
          TObject *Sender);
        void __fastcall WriteFLSflashfiletophone1Click(TObject *Sender);
        void __fastcall Createmapfromlog1Click(TObject *Sender);
        void __fastcall Decryptmapfromfile1Click(TObject *Sender);
        void __fastcall Decryptmapfromphone1Click(TObject *Sender);
        void __fastcall saveconfigurationonexitClick(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall Backupbatteryparametersfromphone1Click(
          TObject *Sender);
        void __fastcall Restorebatteryparametersfromfile1Click(
          TObject *Sender);
        void __fastcall ReadflashfromphoneviaBFBandstoreasFLS1Click(
          TObject *Sender);
        void __fastcall Createlogfromphone1Click(TObject *Sender);
        void __fastcall Createmapsfromlogs1Click(TObject *Sender);
        void __fastcall ReadandpatchbootcoreinFLSformat1Click(
          TObject *Sender);
        void __fastcall Decrypt1Click(TObject *Sender);
        void __fastcall Getdongleinfo1Click(TObject *Sender);
        void __fastcall Uploaddongleinfo1Click(TObject *Sender);
        void __fastcall Updatedongle1Click(TObject *Sender);
        void __fastcall Testdongle1Click(TObject *Sender);
        void __fastcall comport_RTSClick(TObject *Sender);
        void __fastcall comport_DTRClick(TObject *Sender);
        void __fastcall boottype1Click(TObject *Sender);
        void __fastcall boottype2Click(TObject *Sender);
        void __fastcall boottype3Click(TObject *Sender);
private:	// User declarations
        void __fastcall runfreia(char *arglist);
        void __fastcall setredirection(eREDIRECTION_TYPE redirectiontype, bool force);
        void __fastcall setupopendialog(char *ext,char *filter);
        void __fastcall checkKNKemulate(void);
        void __fastcall checkFLSemulate(void);
        bool __fastcall getaddresses(bool Bbinary,
                                     char *pathname,char *title,
                                     int *startaddr,
                                     int *endaddr,
                                     int *startofs,
                                     int *countfile);
        void __fastcall Unlock(bool Bshowmap, bool BnoMAP, bool Bnoupdate);
        void __fastcall Decrypt (bool BfromMAP);
        bool __fastcall buildstartofcommand(char *cmdline);
        bool __fastcall getphonemodel(char *model);
        void __fastcall readflash(bool Bbfb);
        void __fastcall writeflash(void);
        void __fastcall CRCcheck(bool Bfix);
        void __fastcall patchbootcore (bool Bfromfile);
public:		// User declarations
        __fastcall Tmainfunctions(TComponent* Owner);
        void __fastcall setcomapp(int U8port, bool force);
        void __fastcall setcomdng (int U8port, bool force);
        void __fastcall setdebuglevel(int U8debuglevel, bool force);
        void __fastcall setcomtype (bool Bdtr, bool Brts, bool force);
        void __fastcall setcomspeed(int U32speed, bool force);
        void __fastcall setboottype(int value, bool force);
        void __fastcall AdjustStatus (char *modelname, int index);                        
};
//---------------------------------------------------------------------------
extern PACKAGE Tmainfunctions *mainfunctions;
//---------------------------------------------------------------------------
#endif
