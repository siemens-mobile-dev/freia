//---------------------------------------------------------------------------

#ifndef dngmntuH
#define dngmntuH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
        TOpenDialog *opendialog;
        TButton *Button1;
        TEdit *devidlo;
        TEdit *devidhi;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TEdit *version;
        TLabel *Label4;
        TEdit *blocked;
        TLabel *Label5;
        TEdit *misusage_counter;
        TButton *Button2;
        TMemo *memo;
        TButton *Button3;
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall Button2Click(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall Button3Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TForm1(TComponent* Owner);
        void __fastcall setupopendialog (char *ext, char *filter);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
