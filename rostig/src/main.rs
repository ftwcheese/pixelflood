use std::io::prelude::*;
use std::net::{SocketAddr, TcpStream, ToSocketAddrs};
use std::time::Duration;

use image::{GenericImageView, ImageReader};

#[inline]
fn append_str(x: u32, y: u32, color: u32) -> String {
    format!("PX {} {} {:x}\n", x, y, color)
}

fn send_offset(stream: &mut TcpStream, x: u32, y: u32) {
    stream.write_fmt(format_args!("OFFSET {} {}\n", x, y)).unwrap();
}

// struct PairU32 {
//     width: u32,
//     height: u32
// }
// 
// fn get_size(stream: &mut TcpStream) -> PairU32 {
// 
//     let mut buf = String::new();
//     stream.read_to_string(&mut buf).expect("Error reading from stream");
//     
//     // Expect sth. like SIZE 123 456
// 
//     let elements: Vec<&str> = buf.split(" ").collect();
//     let width = elements[1].parse::<u32>().expect("Error parsing size return value width");
//     let height = elements[2].parse::<u32>().expect("Error parsing size return value height");
// 
//     PairU32 { width, height }
// }

const OFFSET: [u32; 2] = [ 0, 500 ];

fn main() -> std::io::Result<()> {
    let mut addr = "wall.c3pixelflut.de:1337".to_socket_addrs().unwrap();
    let addr_first = addr.next().unwrap();
    let mut stream = TcpStream::connect_timeout(&addr_first, Duration::from_secs(3)).expect("Connection fault");

    let img = ImageReader::open("Shrek_64.png").expect("Image loading/parsing error").decode().unwrap();

    // let size = get_size(&mut stream);
   // assert!(img.width() + OFFSET[0] < size.width, "Image width violated");
   // assert!(img.height() + OFFSET[1] < size.height, "Image height violated");

    send_offset(&mut stream, OFFSET[0], OFFSET[1]);

    let pixels_cpy: Vec<_> = img.pixels().clone().collect();
    let mut output_str = "".to_string();


    for (x, y, color) in pixels_cpy.iter() {
        let color_channels = color.0;
        let color_u32: u32 = 
            (color_channels[0] as u32) << 16 | 
            (color_channels[1] as u32) << 8 | 
            (color_channels[2] as u32);
        output_str += append_str(*x + OFFSET[0], *y + OFFSET[1], color_u32).as_str();
    }

    let output_bytes = output_str.as_bytes();

    loop {
        // write!(stream, "{}", output_str).unwrap();
        stream.write(output_bytes).unwrap();
    }

    // stream.read(&mut [0; 128])?;
} // the stream is closed here