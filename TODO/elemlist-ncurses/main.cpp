#include <iostream>
#include <memory>
#include <vector>
#include <tuple>
#include <sstream>
#include <algorithm>


#include <locale>

#include <random>
#include <chrono>

#include <ncurses.h>
#include <form.h>

#define NBLINE 50
#define KEY_ALT_BACKSPACE 127

class _gui {
public:
    
    typedef std::tuple<std::string, std::string, int, float> Line_t;
    
    explicit _gui();
    virtual ~_gui();
    
    void init_color();
    
    void refresh_win() const;
    
    void handlemouseev();
    
    void draw_win();
    void add_scroll();
    void kdb(const wchar_t c_i);
    
    void loop();
    void update_form();
    void update_status();
    void update_mark();
    void toggle_mark();
    
    void scroll_up();
    void scroll_down();
    void scroll_uup();
    void scroll_ddown();
    void scroll_end();
    void scroll_beg();
    
    void show_progress() const;
    void show_info();
    void hide_overlay();
    
    bool storeinfo(); 
    bool save();
    bool delline();
    
    void set_data(const Line_t&& line_t);
    void set_data(const std::vector<Line_t>&& line_v);
    void set_scope(const std::pair<int, int> &scope_p);
    void set_scope(int i);
    void set_title(const std::string& title_str);
    
    const std::string get_title() const;
    int get_linenb() const;
    int get_linelistsize() const;
    int get_posinscope() const;
    int get_pos() const;
    int get_scopesize() const;
    int get_fieldlistsize() const;
    
    const std::vector<_gui::Line_t> select_data(const std::pair<int, int> scope_p) const;
    
private:
    std::string sTitle;
    
    WINDOW* win_p;
    WINDOW* header_p;
    WINDOW* footer_p;
    WINDOW* dialog_p;
    
    MEVENT mouseev;
    
    std::vector<Line_t> linelist_v;
    std::vector<FIELD* > field_v;
    FORM* Form;
    
    const std::string cmd_str;
    
    int nbline_i;
    int linelistsize_i;
    int fieldlistsize_i;
    int scopesize_i;
    int pos_i;
    int posinscope_i;
    
    std::pair<int,int> scope_p;
    
    bool isInfoOn;
    
    const std::string itorom(int val) const;
    
};


int main(int argc, char **argv) {
    
    _gui gui;
    
    gui.init_color();
    
    gui.set_title("elemlist");
    
    gui.set_data({"@!", "Fe", 2, 6000});
    gui.set_data({"#", "Fe", 2, 6500});
    
    for(int i=0; i<NBLINE-3;i++)
        gui.set_data({"!", "Fe", 1, i});
    
    gui.set_data({"%!", "Fe", 4, 7000});
    
    gui.add_scroll();
    gui.draw_win();
    gui.show_progress();
    gui.update_mark();
    gui.update_status();
    
    gui.refresh_win();
    
    int c_i=0;
    while(c_i != 'q') {
        c_i=getch();
        gui.kdb(c_i);
        
        #ifdef NCURSES_MOUSE_VERSION
        
        #endif        
        gui.draw_win();
        gui.refresh_win();
    }
    
    return EXIT_SUCCESS;
}

// ******************************************

_gui::_gui(): 
cmd_str("F1: help - (q)uit"),
nbline_i(0),
linelistsize_i(0),
fieldlistsize_i(0),
scopesize_i(0),
pos_i(0),
posinscope_i(0),
scope_p({0, 0}),
isInfoOn(false) 
{
    setlocale(LC_ALL, "");
    
    initscr();
    start_color();
    cbreak();
    noecho();
    curs_set(2);
    
    keypad(stdscr, true);
    
    #ifdef NCURSES_MOUSE_VERSION
    mousemask(ALL_MOUSE_EVENTS,NULL);
    #endif
    
    this->win_p =    subwin(stdscr, LINES-11, COLS-8, 5, 3);
    this->header_p = subwin(stdscr, 3,        COLS-4, 1, 2);
    this->footer_p = subwin(stdscr, 3,        COLS-4, LINES-4, 2);
    this->dialog_p = newwin(LINES/3,  COLS/3, 7, COLS/3);
    
    if (this->win_p == nullptr || 
        this->header_p == nullptr || 
        this->footer_p == nullptr ||
        this->dialog_p== nullptr) {
        printw("error");
    beep();
    exit(EXIT_FAILURE);
        }
        
        scrollok(win_p, true);
}

void _gui::init_color() {
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    init_pair(5, COLOR_WHITE, COLOR_BLUE);
    init_pair(6, COLOR_BLUE, COLOR_GREEN);    
    init_pair(7, COLOR_GREEN, COLOR_BLACK);
}

// ******************************************


void _gui::draw_win() {
    
    wclrtoeol(this->header_p);
    wclrtoeol(this->footer_p);
    
    box(this->header_p, ACS_VLINE, ACS_HLINE);
    box(this->footer_p, ACS_VLINE, ACS_HLINE);
    box(this->dialog_p, ACS_VLINE, ACS_HLINE);
    
    wbkgd(this->dialog_p, COLOR_PAIR(2));
    wbkgd(this->header_p, COLOR_PAIR(2));
    
    wattron(this->header_p, A_BOLD);
    mvwprintw(this->header_p, 1, COLS/2-5, this->get_title().c_str());
    wattroff(this->header_p, A_BOLD);
    
    mvwprintw(this->footer_p, 1, getmaxx(this->footer_p)-this->cmd_str.size()-2, this->cmd_str.c_str());
    mvwprintw(footer_p, 1, 1, "             ");
    mvwprintw(this->footer_p, 1, 1, "pos: ");
    wattron(this->footer_p, A_BOLD);
    mvwprintw(this->footer_p, 1, 6, "%d", get_pos()/3+1);
    wattroff(this->footer_p, A_BOLD);
    wprintw(this->footer_p, "/%d", this->get_linelistsize());
    refresh_win();
}


void _gui::add_scroll() {
    set_scope(0);
    
    if (!this->linelist_v.empty()) {
        
        auto [min_i, max_i]=this->scope_p;
        
        this->field_v.clear();
        
        for(int i=0; i<this->get_scopesize(); i++) {
            this->field_v.emplace_back(new_field(1, 2, i+5, 24, 0, 0));
            this->field_v.emplace_back(new_field(1, 3, i+5, 28, 0, 0));
            this->field_v.emplace_back(new_field(1, 10, i+5, 36, 0, 0));
        }
        
        //this->field_v.emplace_back(nullptr);
        
        for(int i=0; i<field_v.size(); i+=3) {
            set_field_fore(field_v[i], COLOR_PAIR(1));
            set_field_back(field_v[i], COLOR_PAIR(1));
            field_opts_on(field_v[i], O_AUTOSKIP);
            
            set_field_fore(field_v[i+1], COLOR_PAIR(4));
            set_field_back(field_v[i+1], COLOR_PAIR(4));
            field_opts_on(field_v[i+1], O_AUTOSKIP);
            
            set_field_fore(field_v[i+2], COLOR_PAIR(3));
            set_field_back(field_v[i+2], COLOR_PAIR(3));
            field_opts_on(field_v[i+2], O_AUTOSKIP);
        }
        
        for(int i=min_i, j=0; i<max_i; i++, j+=3)   {
            auto [a,b,c,d]=linelist_v[i];
            set_field_buffer(field_v[j], 0, b.c_str());
            set_field_buffer(field_v[j+1], 0, this->itorom(c).c_str());
            set_field_buffer(field_v[j+2], 0, std::to_string(d).c_str());
        }
        
        this->Form=new_form(static_cast<FIELD**>(this->field_v.data()));
        
        set_current_field(Form, field_v[0]);
        set_form_win(this->Form, this->win_p);
        
        if (post_form(this->Form)!=E_OK) {
            printw("error");
            beep();
            exit(EXIT_FAILURE); 
        }
    }
}

// ******************************************

void _gui::refresh_win() const {
    wrefresh(this->win_p);
    wrefresh(this->header_p);
    wrefresh(this->footer_p);
}

// ******************************************

void _gui::kdb(const wchar_t c_i) {
    std::stringstream ssS;
    
    switch (c_i) {
        case ('o' & 0x1f):
            save();
            break;
        case ('x' & 0x1f):
            delline();
            break;
        case '\n':
            if (storeinfo()) form_driver(this->Form, REQ_NEXT_FIELD);
            else form_driver(this->Form, REQ_BEG_LINE);
            break;
        case KEY_END:
            scroll_end();
            break;
        case KEY_BEG:
            scroll_beg();
            break;
        case KEY_DOWN:
            this->scroll_down();
            this->show_progress();
            break;
        case KEY_UP:
            this->scroll_up();
            this->show_progress();
            break;
        case KEY_NPAGE:
            this->scroll_ddown();
            this->show_progress();
            break;
        case KEY_PPAGE:
            this->scroll_uup();
            this->show_progress();
            break;
        case KEY_LEFT:
            form_driver(this->Form, REQ_PREV_CHAR);
            break;
        case KEY_RIGHT:
            form_driver(this->Form, REQ_NEXT_CHAR);            
            break;
        case '\t':
            form_driver(this->Form, REQ_NEXT_FIELD);
            form_driver(this->Form, REQ_END_LINE);
            break;
        case KEY_BACKSPACE:
            form_driver(this->Form, REQ_DEL_PREV);
            break;
        case KEY_ALT_BACKSPACE:
            form_driver(this->Form, REQ_DEL_PREV);
            break;
        case KEY_DC:
            form_driver(this->Form, REQ_DEL_CHAR);
            break;
        case 'n':
            form_driver(this->Form, REQ_NEXT_PAGE);
            form_driver(this->Form, REQ_END_LINE);
            break;    
        case ' ':
            toggle_mark();
            update_mark();
            update_status();
            break;
        case KEY_F(1):
            show_info();
            break;
        case KEY_RESIZE:
            clrtobot();
            clrtoeol();
            refresh();
            refresh_win();
            update_form();
            update_mark();
            update_status();
            break;
        case KEY_MOUSE:
            handlemouseev();
            break;
        default:
            form_driver(this->Form, c_i);
            break;
    }
}

// ******************************************

const std::string _gui::get_title() const {
    if (this->sTitle.empty())  return " ";
    return this->sTitle;
}

int _gui::get_linenb()        const { return this->nbline_i; }
int _gui::get_scopesize()     const { return std::min(getmaxy(this->win_p), get_linelistsize()); }
int _gui::get_linelistsize()  const { return this->linelist_v.size(); }
int _gui::get_posinscope()    const { return this->posinscope_i%get_scopesize(); }
int _gui::get_fieldlistsize() const { return this->field_v.size(); }

void _gui::set_data(const _gui::Line_t&& line_t) { this->linelist_v.emplace_back(line_t); this->linelistsize_i=this->linelist_v.size(); this->nbline_i=this->linelist_v.size();}
void _gui::set_data(const std::vector< _gui::Line_t >&& line_v) { this->linelist_v=line_v; this->linelistsize_i=this->linelist_v.size(); this->nbline_i=this->linelist_v.size();}
void _gui::set_title(const std::string& sTitle) { this->sTitle=sTitle; }
void _gui::set_scope(const std::pair<int, int> &scope_p) { this->scope_p=scope_p; }

void _gui::set_scope(int val) {
    int scope=get_scopesize()-1;
    int nbval=get_linelistsize();
    int nbscope=(nbval)/scope;
    
    int min_i=static_cast<int>(val/scope)*scope;
    int max_i=static_cast<int>(val/scope+1)*scope;
    
    if (max_i>get_linelistsize()) {
        min_i=nbscope*scope;
        max_i=nbval;
    }

    set_scope({min_i, max_i});
}

// ******************************************

_gui::~_gui() {
    flash();
    unpost_form(this->Form);
    field_v.clear();
    endwin();
}

// ******************************************

const std::string _gui::itorom(int val_i) const {
    if (val_i<4000) {
        int res_i=val_i;
        std::stringstream ssS;
        int num[]{1,4,5,9,10,40,50,90,100,400,500,900,1000}; 
        std::string symb_s[]{"I","IV","V","IX","X","XL","L","XC","C","CD","D","CM","M"}; 
        int i=12;
        while(res_i>0) { 
            int div = res_i/num[i]; 
            res_i = res_i%num[i]; 
            while(div--) ssS << symb_s[i]; 
            i--; 
        } 
        return ssS.str();
    }
    return "Z";
}

const std::vector<_gui::Line_t> _gui::select_data(const std::pair<int, int> scope_p) const {
    auto [min, max]=scope_p;
    if (min<0) min=0;
    if (max>linelist_v.size()-1) max=linelist_v.size()-1;
    
    std::vector<Line_t> trimed_l;
    for(int i=min; i<max+1;i++) trimed_l.emplace_back(linelist_v[i]);
    return trimed_l;
}   

void _gui::update_form() {    
    const auto [min_i, max_i]=this->scope_p;
    
    for(int i=min_i, j=0; i<max_i; i++, j+=3)   {
        auto [a,b,c,d]=linelist_v[i];
        set_field_buffer(field_v[j], 0, b.c_str());
        set_field_buffer(field_v[j+1], 0, this->itorom(c).c_str());
        set_field_buffer(field_v[j+2], 0, std::to_string(d).c_str());
    }
    
    for(auto &field: field_v) field_opts_on(field, O_VISIBLE);    
    for(int i=3*(max_i-min_i); i<3*getmaxy(win_p); i++) field_opts_off(field_v[i], O_VISIBLE);   
}

void _gui::update_mark() {
    const auto [min_i, max_i]=this->scope_p;
    
    for(int i=0; i<getmaxy(this->win_p); i++)
        mvwprintw(this->win_p, i, 0, "               ");
    
    for(int i=min_i; i<max_i; i++) {
        std::string mark_s=std::get<0>(this->linelist_v[i%(max_i-min_i)+min_i]);
        
        std::string sS0, sS1 = " ]";
        if (mark_s!="@!" && mark_s!="%!") sS0 = "[  ";
        else sS0 = "[ ";
        
        wattron(this->win_p, COLOR_PAIR(2));
        mvwprintw(this->win_p, i%(max_i-min_i), 0, "%d", i%(max_i-min_i)+min_i+1);
        wattroff(this->win_p, COLOR_PAIR(2));
        
        mvwprintw(this->win_p, ((i)%(max_i-min_i)), 9, sS0.c_str());
        wattron(this->win_p, A_BOLD);
        mvwprintw(this->win_p, ((i)%(max_i-min_i)), 11, mark_s.c_str());
        wattroff(this->win_p, A_BOLD);
        mvwprintw(this->win_p, ((i)%(max_i-min_i)), 13, sS1.c_str());
    }   
    
    wattron(this->win_p, COLOR_PAIR(6));
    mvwprintw(this->win_p, (get_pos()/3)%(get_scopesize()-1), 0, "%d", (get_pos()/3+1));
    wattroff(this->win_p, COLOR_PAIR(6));
}

void _gui::update_status() {
    wclrtoeol(this->footer_p);
    
    for(int i=0; i<getmaxx(footer_p); i++)
        mvwprintw(this->footer_p, 1, i, " ");
    
    std::string mark_s=std::get<0>(this->linelist_v[get_pos()/3]);
    
    mvwprintw(this->footer_p, 1, (COLS-3)/3, "[      ]");
    mvwprintw(this->footer_p, 1, (COLS-3)/3+8, "[    ]");
    mvwprintw(this->footer_p, 1, (COLS-3)/3+14, "[    ]");
    
    if (mark_s=="@!") {
        mvwprintw(this->footer_p, 1, (COLS-3)/3, "[");
        wattron(this->footer_p, A_DIM);
        wprintw(this->footer_p,"unbold");
        wattroff(this->footer_p, A_DIM);
        wprintw(this->footer_p,"]");
    }
    if (mark_s=="!") {
        mvwprintw(this->footer_p, 1, (COLS-3)/3+8, "[");
        wattron(this->footer_p, A_BOLD);
        wprintw(this->footer_p,"bold");
        wattroff(this->footer_p, A_BOLD);
        wprintw(this->footer_p,"]");
    }
    if (mark_s=="%%" || mark_s=="%%!" || mark_s=="%!" || mark_s=="@!" ) 
        mvwprintw(this->footer_p, 1, (COLS-3)/3+14, "[temp]");
    if (mark_s=="#") {
        mvwprintw(this->footer_p, 1, (COLS-3)/3+8, "[");
        wattron(this->footer_p, A_DIM);
        wprintw(this->footer_p,"mask");
        wattroff(this->footer_p, A_DIM);
        wprintw(this->footer_p,"]");
    }
    
    if (mark_s==" ")  mvwprintw(this->footer_p, 1, (COLS-3)/3+8, "[    ]");
    
    if (get_pos()==0) {
        wattron(footer_p, A_BLINK);
        wattron(footer_p, A_BOLD);
        mvwprintw(footer_p, 1, 1*(COLS-3)/5, " [BOF] ");
        wattroff(footer_p, A_BOLD);
        wattroff(footer_p, A_BLINK);
        beep();
    }
    
    if (get_pos()>3*(get_linelistsize()-2)) {
        wattron(footer_p, A_BLINK);
        wattron(footer_p, A_BOLD);
        mvwprintw(footer_p, 1, 1*(COLS-3)/5, " [EOF] ");
        wattroff(footer_p, A_BOLD);
        wattroff(footer_p, A_BLINK);
        beep();
    }
}

void _gui::toggle_mark() {
    std::string rot_s(std::get<0>(this->linelist_v[(get_pos())/3]));
    if      (rot_s==" ")  rot_s="%%";
    else if (rot_s=="%%") rot_s="#";
    else if (rot_s=="#")  rot_s="!";
    else if (rot_s=="!")  rot_s="@!";
    else if (rot_s=="@!") rot_s=" ";
    else                  rot_s=" ";
    
    std::get<0>(this->linelist_v[(get_pos())/3])=rot_s;
}

void _gui::scroll_up() {
    this->pos_i-=3;
    if (this->pos_i<0) this->pos_i=0;
    
    set_scope(this->pos_i/3);
    set_current_field(Form, this->field_v[get_pos()%(3*get_scopesize()-3)]);
    
    update_mark();
    update_status();
    update_form();
}

void _gui::scroll_uup() {
    this->pos_i-=static_cast<int>(get_scopesize()/3)*9+3;
    if (this->pos_i<0) this->pos_i=0;
    
    set_scope(this->get_pos()/3);
    set_current_field(Form, this->field_v[this->get_pos()%(3*get_scopesize()-3)]);
    
    update_mark();
    update_status();
    update_form();
}

void _gui::scroll_down() {
    this->pos_i+=3;
    
    if (this->pos_i>3*(get_linelistsize()-2))  this->pos_i=3*(get_linelistsize()-1);
    
    set_scope(this->pos_i/3);
    
    set_current_field(Form, this->field_v[get_pos()%(3*(get_scopesize()-1))]);
    
    auto [min, max]= scope_p;
    mvwprintw(this->win_p, 1, getmaxx(win_p)-100, "pos:%d in [%d, %d] llsize:%d scsize:%d fdsize:3*%d\t", pos_i, min, max, get_linelistsize(), get_scopesize(), get_fieldlistsize()/3);

    update_form();
    update_status();
    update_mark();
}

void _gui::scroll_ddown() {
    this->pos_i+=static_cast<int>(get_scopesize()/3)*9+3;
    
    if (this->pos_i>3*get_linelistsize()-3) this->pos_i=3*get_linelistsize()-3;
    
    set_scope(this->pos_i/3);
    set_current_field(Form, this->field_v[this->pos_i%(3*(get_scopesize()-1))]);
    
    update_form();
    update_status();
    update_mark();
}

void _gui::scroll_beg() {
    this->pos_i=0;
    
    set_scope(0);
    set_current_field(Form, this->field_v[0]);
    
    update_mark();
    update_status();
    update_form();
}

void _gui::scroll_end() {
    this->pos_i=get_linelistsize()*3;
    
    if (this->pos_i>3*get_linelistsize()-3) this->pos_i=3*get_linelistsize()-3;
    
    set_scope(this->pos_i/3);
    set_current_field(Form, this->field_v[this->pos_i%(3*(get_scopesize()-1))]);
    
    update_form();
    update_status();
    update_mark();
}

int _gui::get_pos() const {
    int res_i=this->pos_i;
    if (res_i<0) res_i=0;
    if (res_i>3*get_linelistsize()) res_i=3*get_linelistsize();
    return res_i; 
}

void _gui::show_progress() const {
    mvwprintw(win_p, 0, getmaxx(win_p)-4, "---" ); 
    mvwprintw(win_p, getmaxy(win_p)-1, getmaxx(win_p)-4, "---" );
    
    for(int i=1; i<getmaxy(win_p)-1; i++)
        mvwprintw(win_p, i, getmaxx(win_p)-4, " | " ); 
    
    mvwprintw(this->win_p, 1+((get_scopesize()-2)*(get_pos())/get_linelistsize()/3)%(get_scopesize()), getmaxx(win_p)-4, ">");
    
    wattron(this->win_p, A_BLINK);
    wattron(this->win_p, COLOR_PAIR(7));
    mvwprintw(this->win_p, 1+((get_scopesize()-2)*(get_pos())/get_linelistsize()/3)%(get_scopesize()), getmaxx(win_p)-3, "|");
    wattroff(this->win_p, COLOR_PAIR(7));
    wattroff(this->win_p, A_BLINK);
    
    mvwprintw(this->win_p, 1+((get_scopesize()-2)*(get_pos())/get_linelistsize()/3)%(get_scopesize()), getmaxx(win_p)-2, "<");
    
    wrefresh(this->win_p);
}


void _gui::show_info() {
    int count_i=2;
    auto printkey=[&](const std::tuple<std::string, std::string> & key) {
        mvwprintw(this->dialog_p, count_i, 2, std::get<0>(key).c_str() );
        mvwprintw(this->dialog_p, count_i, 2*getmaxx(this->dialog_p)/4,std::get<1>(key).c_str() );
        count_i++;
    };
    
    mvwprintw(this->dialog_p, 0, 0, "Shortcuts");
    
    printkey({"F1: Help", " "});
    count_i++;
    printkey({"^n: New Line", "^d: Delete Line"});
    printkey({"^o: Save", " " });
    printkey({" q: Quit", " "});
    count_i++;
    printkey({"SPACE: Toggle Mark", "ENTER: Validate field"});
    printkey({" ", " "});
    
    overlay(stdscr,this->dialog_p);
    wrefresh(this->dialog_p);
}

void _gui::hide_overlay() {
    werase(this->dialog_p);
    wrefresh(this->dialog_p);
    this->isInfoOn=false;
}

bool _gui::storeinfo() {
    wattron(this->footer_p, COLOR_PAIR(7));
    mvwprintw(this->footer_p, 1, 2*COLS/3, "[stored]");
    wattroff(this->footer_p, COLOR_PAIR(7));
    return true;
}

bool _gui::save() {
    
    wattron(this->footer_p, COLOR_PAIR(7));
    mvwprintw(this->footer_p, 1, 2*COLS/3, "[saved]");
    wattroff(this->footer_p, COLOR_PAIR(7));
    return true;
}

bool _gui::delline() {
    
    if (get_linelistsize()>2) {
        this->linelist_v.erase(this->linelist_v.begin()+get_pos()/3, this->linelist_v.begin()+get_pos()/3+1); 
        
        if (this->pos_i>3) pos_i-=3;
        
        set_scope(this->pos_i/3);
        
        set_current_field(Form, this->field_v[get_pos()%(3*get_scopesize()-3)]);
        
//         auto [min, max]=this->scope_p;
        
        wattron(this->footer_p, COLOR_PAIR(7));
        mvwprintw(this->footer_p, 1, 2*COLS/3, "[deleted]");
        wattroff(this->footer_p, COLOR_PAIR(7));
        
        update_mark();
        update_status();
        update_form();
    }
    
    return true;
}

void _gui::handlemouseev() {
    #ifdef NCURSES_MOUSE_VERSION
    
    getmouse(&this->mouseev);
    auto [min, max]=this->scope_p;
    
    if (this->mouseev.y<get_scopesize()+4 && this->mouseev.y>4)  {
        this->pos_i=(min+(this->mouseev.y%(get_scopesize()+5)-5))*3;
        set_current_field(Form, this->field_v[3*(this->mouseev.y%(get_scopesize()+5)-5)]);
    }
    
    wattron(this->footer_p, COLOR_PAIR(7));
    mvwprintw(this->footer_p, 1, 1*COLS/4, "(%d, %d): %d %d", this->mouseev.x, this->mouseev.y, min+(this->mouseev.y%(get_scopesize()+5)-5), pos_i);
    wattroff(this->footer_p, COLOR_PAIR(7));
    
    draw_win();
    update_mark();
    update_status();
    update_form();
    
    #endif
}
