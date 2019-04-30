package ija.ija2018.homework2.common;

public interface Figure{
    public enum Roles{
        Pawn,Rook
    }

    Roles getRole();
    void zmena(Field f);
    String getState();
    int getColor();
    Field getCoordinates();
}
