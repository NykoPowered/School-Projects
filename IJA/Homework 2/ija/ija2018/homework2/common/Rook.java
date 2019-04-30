package ija.ija2018.homework2.common;

import ija.ija2018.homework2.game.Board;
import java.util.Stack;

public class Rook implements Figure {

    private int farba;
    public Field coordinates;
    private Figure.Roles role;

    public Rook(int i,Field f){
        this.role=Roles.Rook;
        this.farba = i;
    }

    public Field getCoordinates(){
        return this.coordinates;
    }

    @Override
    public int getColor(){
        return this.farba;
    }

    public Roles getRole(){
        return this.role;
    }

    @Override
    public void zmena(Field f){
        this.coordinates=f;
    }

    @Override
    public String getState(){
        if(this.farba == 0){
            return "V[W]" + coordinates.getcol() +":"+ coordinates.getrow();
        } else{
            return "V[B]" + coordinates.getcol() +":"+ coordinates.getrow();
        }

    }
}
