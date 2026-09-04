import zlib
import struct
import math

def generate_telegram_png(filename, size=240):
    # Colors
    # Background Telegram blue: #24A1DE = (36, 161, 222)
    # Shading 1: #c8daea = (200, 218, 234)
    # Shading 2: #a9c9dd = (169, 201, 221)
    # White: (255, 255, 255)
    
    # We will rasterize the official polygon paths
    # Center circle:
    cx = size / 2.0
    cy = size / 2.0
    r = (size / 2.0) - 2.0
    
    # Scale from 240x240 viewBox
    s = size / 240.0
    
    # Polygons:
    # 1. Main white wing:
    # m100.04 144.41 48.36 35.729c5.5185 3.0449 9.5014 1.4684 10.876-5.1235l19.685-92.763c2.0154-8.0802-3.0801-11.745-8.3594-9.3482l-115.59 44.571c-7.8901 3.1647-7.8441 7.5666-1.4382 9.528l29.663 9.2583 68.673-43.325c3.2419-1.9659 6.2173-0.90899 3.7752 1.2584z
    # Let's define the key vertices of the telegram paper plane:
    # Tip: (53.6, 127.2)
    # Top right: (177.5, 75.3)
    # Bottom right: (159.3, 175.0)
    # Fold bottom: (116.0, 155.6)
    # Inner fold: (98.0, 132.2)
    # Bottom tip: (98.0, 175.0)
    
    # Let's create an RGBA buffer
    raw_data = []
    
    # Path triangles for paper plane:
    # Triangle 1 (Bottom flap 1 - c8daea): (98, 175), (82, 132.2), (170, 80)
    # Triangle 2 (Bottom flap 2 - a9c9dd): (98, 175), (114, 159.4), (100, 144.4)
    # Main body: (53.6, 127.2) -> (177.5, 75.3) -> (159.3, 175.0) -> (100, 144.4) -> (82, 132.2)
    
    def point_in_tri(px, py, p1, p2, p3):
        def sign(p1, p2, p3):
            return (p1[0] - p3[0]) * (p2[1] - p3[1]) - (p2[0] - p3[0]) * (p1[1] - p3[1])
        d1 = sign((px, py), p1, p2)
        d2 = sign((px, py), p2, p3)
        d3 = sign((px, py), p3, p1)
        has_neg = (d1 < 0) or (d2 < 0) or (d3 < 0)
        has_pos = (d1 > 0) or (d2 > 0) or (d3 > 0)
        return not (has_neg and has_pos)

    def point_in_poly(px, py, poly):
        n = len(poly)
        inside = False
        p1x, p1y = poly[0]
        for i in range(n + 1):
            p2x, p2y = poly[i % n]
            if py > min(p1y, p2y):
                if py <= max(p1y, p2y):
                    if px <= max(p1x, p2x):
                        if p1y != p2y:
                            xinters = (py - p1y) * (p2x - p1x) / (p2y - p1y) + p1x
                        if p1x == p2x or px <= xinters:
                            inside = not inside
            p1x, p1y = p2x, p2y
        return inside

    # Scaled coordinates in 240x240
    # Official Telegram airplane polygon definition:
    # 1. Main body (white):
    poly_main = [
        (54.0 * s, 126.0 * s),
        (177.0 * s, 74.0 * s),
        (159.0 * s, 175.0 * s),
        (100.0 * s, 144.4 * s)
    ]
    # Center fold crease white:
    poly_fold = [
        (100.0 * s, 144.4 * s),
        (177.0 * s, 74.0 * s),
        (83.0 * s, 132.0 * s)
    ]
    # Flap 1 (shading):
    poly_flap1 = [
        (98.0 * s, 175.0 * s),
        (83.0 * s, 132.0 * s),
        (120.0 * s, 140.0 * s)
    ]
    # Flap 2 (light blue fold):
    poly_flap2 = [
        (98.0 * s, 175.0 * s),
        (120.0 * s, 140.0 * s),
        (100.0 * s, 144.4 * s)
    ]
    
    # Let's supersample 2x for smooth edges:
    for y in range(size):
        row = [chr(0)] # PNG filter byte 0
        for x in range(size):
            # Supersample 2x2
            cr, cg, cb, ca = 0, 0, 0, 0
            for sy in [0.25, 0.75]:
                for sx in [0.25, 0.75]:
                    px = x + sx
                    py = y + sy
                    dx = px - cx
                    dy = py - cy
                    dist = math.sqrt(dx*dx + dy*dy)
                    
                    if dist <= r:
                        # Inside circle
                        # Default circle color: Telegram blue #24A1DE
                        pr, pg, pb, pa = 36, 161, 222, 255
                        
                        # Check airplane parts:
                        if point_in_poly(px, py, poly_flap1):
                            pr, pg, pb = 200, 218, 234
                        if point_in_poly(px, py, poly_flap2):
                            pr, pg, pb = 169, 201, 221
                        if point_in_poly(px, py, poly_fold):
                            pr, pg, pb = 255, 255, 255
                        if point_in_poly(px, py, poly_main):
                            pr, pg, pb = 255, 255, 255
                            
                        # Edge anti-aliasing for circle
                        if dist > r - 1.0:
                            alpha = int(255 * (r - dist))
                            if alpha < 0: alpha = 0
                            if alpha > 255: alpha = 255
                            pa = alpha
                    else:
                        pr, pg, pb, pa = 0, 0, 0, 0
                        
                    cr += pr
                    cg += pg
                    cb += pb
                    ca += pa
                    
            row.append(chr(cr // 4))
            row.append(chr(cg // 4))
            row.append(chr(cb // 4))
            row.append(chr(ca // 4))
        raw_data.append("".join(row))
        
    compressed = zlib.compress("".join(raw_data), 9)
    
    def chunk(tag, data):
        return struct.pack(">I", len(data)) + tag + data + struct.pack(">I", zlib.crc32(tag + data) & 0xffffffff)
        
    png = "\x89PNG\r\n\x1a\n"
    png += chunk("IHDR", struct.pack(">IIBBBBB", size, size, 8, 6, 0, 0, 0))
    png += chunk("IDAT", compressed)
    png += chunk("IEND", "")
    
    with open(filename, "wb") as f:
        f.write(png)
    print("Generated: " + filename + " (" + str(len(png)) + " bytes)")

if __name__ == "__main__":
    generate_telegram_png("d:/Telegram/assets/images/telegram_logo.png", 240)
