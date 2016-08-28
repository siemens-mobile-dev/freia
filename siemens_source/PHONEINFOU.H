//---------------------------------------------------------------------------

#ifndef phoneinfouH
#define phoneinfouH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "UPlasmaForm.hpp"
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class Tphoneinfo : public TForm
{
__published:	// IDE-managed Components
        TPlasmaForm *PlasmaForm1;
        TGroupBox *GroupBox1;
        TLabel *fwaddress;
        TLabel *eeprom1;
        TLabel *bootcore;
        TComboBox *phonemodel;
        TLabel *Label1;
        TLabel *eeprom2;
        void __fastcall phonemodelChange(TObject *Sender);
        void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
private:	// User declarations
public:		// User declarations
        __fastcall Tphoneinfo(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE Tphoneinfo *phoneinfo;
//---------------------------------------------------------------------------
#endif
