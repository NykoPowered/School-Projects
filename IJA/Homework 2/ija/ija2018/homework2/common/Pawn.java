package ija.ija2018.homework2.common;

import ija.ija2018.homework2.game.Board;

import java.util.Stack;

public class Pawn implements Figure {
    private int farba;
    public Field coordinates;
    private Figure.Roles role;

    public Pawn(int i,Field f){
        this.role=Roles.Pawn;
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
            return "P[W]" + coordinates.getcol() + ":" + coordinates.getrow();
        } else{
            return "P[B]" + coordinates.getcol() + ":" + coordinates.getrow();
        }

    }
}
