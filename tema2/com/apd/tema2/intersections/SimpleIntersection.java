package com.apd.tema2.intersections;

import java.util.ArrayList;
import java.util.Deque;
import java.util.concurrent.CyclicBarrier;
import java.util.concurrent.Semaphore;

import com.apd.tema2.entities.Intersection;

public class SimpleIntersection implements Intersection {
    // Define your variables here.
    public static int N;
    public static int T;
    public static int X;
    public static int carsAcces;
    public static Semaphore sem;
    public static ArrayList<Semaphore> semV2;
    public static ArrayList<Semaphore> semV3;
    public static ArrayList<Semaphore> semV4;
    public static Semaphore semV5;
    public static CyclicBarrier barBenzi;
    public static CyclicBarrier barPermisiune;
    public static CyclicBarrier barMaxEntry;
    public static CyclicBarrier barSem;
    public static CyclicBarrier barrier7;
    public static boolean carGO = true;
    public static CyclicBarrier barTask10;
    public static Deque<Integer> queueTask10;

    public static CyclicBarrier left;
    public static CyclicBarrier right;
    public static CyclicBarrier finalB;

    public static Semaphore semLeft;
    public static Semaphore semRight;
    public static boolean dir;
    public static Object lock;
}
