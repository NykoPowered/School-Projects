/* IJA ukol 1 2019
    @author Nikolas Masica
 */

package ija.ija2018.homework2.common;

import ija.ija2018.homework2.common.Field;
import ija.ija2018.homework2.common.Figure;
import ija.ija2018.homework2.common.*;

public class BoardField implements Field
{
    private int row;
    private int col;
    private Figure figure;
    public Field[] this_field = new Field[8];

    public BoardField(int col, int row)
    {
        this.row = row;
        this.col = col;
    }

    public void addNextField(Field.Direction dirs, Field field)
    {
        if (dirs == Field.Direction.D)
        {
            this_field[0]=field;
        }
        else if (dirs == Field.Direction.L)
        {
            this_field[1]=field;
        }
        else if (dirs == Field.Direction.LD)
        {
            this_field[2]=field;
        }
        else if (dirs == Field.Direction.LU)
        {
            this_field[3]=field;
        }
        else if (dirs == Field.Direction.R)
        {
            this_field[4]=field;
        }
        else if (dirs == Field.Direction.RD)
        {
            this_field[5]=field;
        }
        else if (dirs == Field.Direction.RU)
        {
            this_field[6]=field;
        }
        else if (dirs == Field.Direction.U)
        {
            this_field[7]=field;
        }
    }

    public Field nextField(Field.Direction dirs)
    {
        if (dirs == Field.Direction.D)
        {
            return this_field[0];
        }
        else if (dirs == Field.Direction.L)
        {
            return this_field[1];
        }
        else if (dirs == Field.Direction.LD)
        {
            return this_field[2];
        }
        else if (dirs == Field.Direction.LU)
        {
            return this_field[3];
        }
        else if (dirs == Field.Direction.R)
        {
            return this_field[4];
        }
        else if (dirs == Field.Direction.RD)
        {
            return this_field[5];
        }
        else if (dirs == Field.Direction.RU)
        {
            return this_field[6];
        }
        else if (dirs == Field.Direction.U)
        {
            return this_field[7];
        }
        else {
            return null;
        }
    }



    public boolean put(Figure figure)
    {
        if(this.figure == null){
            figure.zmena(this);
            this.figure = figure;
            return true;
        }else{
            return false;
        }


    }

    public Figure get()
    {
        if (figure == null)
        {
            return null;
        }
        else
        {
            figure.zmena(this);
            return figure;
        }
    }


    public boolean isEmpty()
    {
        if(this.figure == null){
            return true;
        }
        else
        {
            return false;
        }
    }

    public int getrow(){
        return this.row;
    }

    public int getcol(){
        return this.col;
    }

    public boolean remove(Figure figure)
    {

        if(this.figure == figure) {
            figure.zmena(null);
            this.figure = null;
            return true;

        }
        else{
            return false;
        }
    }

}
